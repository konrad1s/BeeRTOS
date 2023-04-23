/******************************************************************************************
 * @brief OS main source file
 * @file BeeRTOS.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS.h"
#include "BeeRTOS_internal.h"
#include "stm32f4xx.h"
#include "os_portable.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_alarm.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

os_task_t *volatile os_task_current;
os_task_t *volatile os_task_next;

static os_task_t *os_tasks[OS_TASK_MAX];
static uint32_t os_ready_mask;
static uint32_t os_delay_mask;

static uint32_t os_isr_counter;

static volatile uint32_t os_tick_counter;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
extern uint32_t* os_port_task_stack_init(void (*task)(void *), void *arg, void *stack_ptr, uint32_t stack_size);
extern void os_cpu_init(void);
extern void os_port_disable_interrupts(void);
extern void os_port_enable_interrupts(void);
extern void os_port_context_switch(void);
extern void os_sched(void);

void os_init(void)
{
    os_ready_mask = 0U;
    os_delay_mask = 0U;
    os_isr_counter = 0U;
    os_tick_counter = 0U;

    for (uint32_t i = 0U; i < OS_TASK_MAX; i++)
    {
        os_tasks[i] = NULL;
    }

    os_cpu_init();
}

void os_run(void)
{
    os_sched();
}

void os_disable_all_interrupts(void)
{
    os_port_disable_interrupts();
    os_isr_counter++;
}

void os_enable_all_interrupts(void)
{
    os_isr_counter--;
    if (os_isr_counter == 0U)
    {
        os_port_enable_interrupts();
    }
}

void os_stack_monitor(void)
{

}

void os_delay(uint32_t ticks)
{
    os_disable_all_interrupts();

    os_task_current->ticks = ticks;
    os_ready_mask &= ~(1 << (os_task_current->priority - 1U));
    os_delay_mask |= (1 << (os_task_current->priority - 1U));
    os_sched();

    os_enable_all_interrupts();
}

void os_task_release(uint32_t task_id)
{
    os_disable_all_interrupts();

    os_ready_mask |= (1 << (task_id - 1U));
    os_delay_mask &= ~(1 << (task_id - 1U));
    os_sched();

    os_enable_all_interrupts();
}

void os_tick(void)
{
    uint32_t mask = os_delay_mask;

    while (mask)
    {
       os_task_t *task = os_tasks[OS_LOG2(mask)];

        task->ticks--;
         if (task->ticks == 0)
         {
              os_ready_mask |= (1 << (task->priority - 1U));
              os_delay_mask &= ~(1 << (task->priority - 1U));
         }
            mask &= ~(1 << (task->priority - 1U));
    }

    os_tick_counter++;
    os_alarm_tick();
}

uint32_t os_get_tick_count(void)
{
    return os_tick_counter;
}

void os_sched(void)
{
    os_disable_all_interrupts();

    if (os_ready_mask == 0U)
    {
        os_task_next = os_tasks[0];
    }
    else
    {
        os_task_next = os_tasks[OS_LOG2(os_ready_mask)];
    }

    if(os_task_current != os_task_next)
    {
        os_port_context_switch();
    }

    os_enable_all_interrupts();
}

void os_task_delete(void)
{
    os_disable_all_interrupts();

    os_ready_mask &= ~(1 << (os_task_current->priority - 1U));
    os_delay_mask &= ~(1 << (os_task_current->priority - 1U));
    os_tasks[os_task_current->priority] = NULL;
    os_sched();

    os_enable_all_interrupts();
}

void os_task_create(os_task_t *task, os_task_handler task_handler,
                    void *stack, uint32_t stack_size, uint8_t priority)
{
    uint32_t *stack_ptr = os_port_task_stack_init(task_handler, NULL, stack, stack_size);

    /* Set stack pointer */
    task->sp = (void *)stack_ptr;

    if(priority > 0U)
    {
        os_ready_mask |= (1 << (priority - 1U));
    }

    task->priority = priority;
    task->ticks = 0U;

    if(priority < OS_TASK_MAX)
    {
        os_tasks[priority] = task; //TODO: assert
    }
}

os_task_t* os_get_current_task(void)
{
    return os_task_current;
}
