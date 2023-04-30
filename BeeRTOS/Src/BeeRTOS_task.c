/******************************************************************************************
 * @brief OS task source file
 * @file BeeRTOS_task.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_assert.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

/*! X-Macro to create task stack array for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
    static os_stack_t name ## _stack[stack];

#define BEERTOS_STACK_VAR BEERTOS_TASK_LIST

BEERTOS_STACK_VAR;

/*! X-Macro to create array of pointers to stack arrays for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
    name ## _stack,

#define BEERTOS_STACK_PTR_VAR BEERTOS_TASK_LIST

static os_stack_t* task_stacks[] = {
    BEERTOS_STACK_PTR_VAR
};

/*! X-Macro to create task control structure for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
    static os_task_t name ## _control;

#define BEERTOS_TASK_CONTROL_VAR BEERTOS_TASK_LIST

BEERTOS_TASK_CONTROL_VAR;

os_task_t *volatile os_task_current;
os_task_t *volatile os_task_next;

static os_task_t *os_tasks[OS_TASK_MAX];
static uint32_t os_ready_mask;
static uint32_t os_delay_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

static inline void os_task_stack_mon(void)
{
    const os_task_t* const task = os_get_current_task();
    const os_stack_t pattern = OS_TASK_STACK_PATTERN;

    if((task_stacks[task->priority][0] != pattern) ||
        (task_stacks[task->priority][1] != pattern) ||
        (task_stacks[task->priority][2] != pattern) ||
        (task_stacks[task->priority][3] != pattern))
    {
        /* TODO - handle stack overflow */
        while(1);
    }
}

#define OS_TASK_STACK_CHECK_BYTE_COUNT 10U
static inline void os_task_stack_mon2(void)
{
    const os_task_t* const task = os_get_current_task();
    const os_stack_t pattern = OS_TASK_STACK_PATTERN;

    for(uint32_t i = 0U; i < OS_TASK_STACK_CHECK_BYTE_COUNT; i++)
    {
        if(task_stacks[task->priority][i] != pattern)
        {
            /* TODO - handle stack overflow */
            while(1);
        }
    }
}

void os_task_init(void)
{
    os_ready_mask = 0U;
    os_delay_mask = 0U;
    for (uint32_t i = 0U; i < OS_TASK_MAX; i++)
    {
        os_tasks[i] = NULL;
    }

    /*! X-Macro to call os_task_create for all tasks */
    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
        os_task_create(&name##_control, cb, name##_stack, sizeof(name##_stack), prio);

    #define BEERTOS_TASK_INIT BEERTOS_TASK_LIST

    BEERTOS_TASK_INIT;

    /*! X-Macro to call os_task_start for all tasks */
    uint8_t task_id = 0U;
    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, prio, stack, autostart, argv)    \
        if (true == autostart)                                      \
        {                           \
            os_task_start(task_id); \
        }                           \
        task_id++;
    
    #define BEERTOS_TASK_START BEERTOS_TASK_LIST

    BEERTOS_TASK_START;
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

bool os_task_start(os_task_id_t task_id)
{
    /* TODO */
    BEERTOS_ASSERT(task_id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task_id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
}

bool os_task_stop(os_task_id_t task_id)
{
    /* TODO */
    BEERTOS_ASSERT(task_id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task_id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
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

void os_task_release(uint32_t task_id)
{
    os_disable_all_interrupts();

    os_ready_mask |= (1 << (task_id - 1U));
    os_delay_mask &= ~(1 << (task_id - 1U));
    os_sched();

    os_enable_all_interrupts();
}

os_task_t* os_get_current_task(void)
{
    return os_task_current;
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

void os_task_tick(void)
{
    uint32_t mask = os_delay_mask;

    while (mask)
    {
       os_task_t* const task = os_tasks[OS_LOG2(mask)];

        task->ticks--;
         if (task->ticks == 0)
         {
              os_ready_mask |= (1 << (task->priority - 1U));
              os_delay_mask &= ~(1 << (task->priority - 1U));
         }
            mask &= ~(1 << (task->priority - 1U));
    }
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
