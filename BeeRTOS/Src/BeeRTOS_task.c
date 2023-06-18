/******************************************************************************************
 * @brief OS task source file
 * @file BeeRTOS_task.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_task.h"
#include "BeeRTOS_assert.h"
#include "BeeRTOS_trace_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define BIT_SET(var, bit)           (var |= (1U << bit))
#define BIT_CLEAR(var, bit)         (var &= ~(1U << bit))

#define BEERTOS_TASK_START(task_id)         (BIT_SET(os_ready_mask, (task_id - 1U)))
#define BEERTOS_TASK_STOP(task_id)          (BIT_CLEAR(os_ready_mask, (task_id - 1U)))

#define BEERTOS_TASK_DELAY_SET(task_id)     (BIT_SET(os_delay_mask, (task_id - 1U)))
#define BEERTOS_TASK_DELAY_CLEAR(task_id)   (BIT_CLEAR(os_delay_mask, (task_id - 1U)))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

/*! X-Macro to create task stack array for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, cb, stack, autostart, argv) \
    static os_stack_t name ## _stack[stack];

#define BEERTOS_STACK_VAR BEERTOS_TASK_LIST()

static os_stack_t os_idle_task_stack[BEERTOS_IDLE_TASK_STACK_SIZE];
BEERTOS_STACK_VAR;

/*! X-Macro to create array of pointers to stack arrays for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, ...) \
    name ## _stack,

#define BEERTOS_STACK_PTR_VAR BEERTOS_TASK_LIST()

static os_stack_t* task_stacks[] = {
    os_idle_task_stack,
    BEERTOS_STACK_PTR_VAR
};

/*! X-Macro to create task control structure for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, ...) \
    static os_task_t name ## _control;

#define BEERTOS_TASK_CONTROL_VAR BEERTOS_TASK_LIST()

static os_task_t os_idle_task_control;
BEERTOS_TASK_CONTROL_VAR;

os_task_t *volatile os_task_current;
os_task_t *volatile os_task_next;

os_task_t *os_tasks[OS_TASK_MAX];

static os_task_mask_t os_ready_mask;
static os_task_mask_t os_delay_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

extern os_stack_t* os_port_task_stack_init(void (*task)(void *), void *arg, void *stack_ptr, uint32_t stack_size);
extern void os_port_context_switch(void);

#if (BEERTOS_USE_TASK_STACK_MONITOR == true)
    #if (BEERTOS_USE_FAST_STACK_MONITOR == true)
        static inline void os_task_stack_mon(void)
        {
            const os_task_t* const task = os_task_current;
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
    #elif (BEERTOS_USE_USER_STACK_MONITOR == true)
        static inline void os_task_stack_mon(void)
        {
            const os_task_t* const task = os_task_current;
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
    #endif /* BEERTOS_USE_USER_STACK_MONITOR */
#endif /* BEERTOS_USE_TASK_STACK_MONITOR */

static void os_task_create(os_task_t *task, os_task_handler task_handler,
                    void *stack, uint32_t stack_size, uint8_t priority, void *argv)
{
    BEERTOS_ASSERT(os_tasks[priority] == NULL, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task != NULL, OS_MODULE_ID_TASK, OS_ERROR_NULLPTR);
    BEERTOS_ASSERT(task_handler != NULL, OS_MODULE_ID_TASK, OS_ERROR_NULLPTR);
    BEERTOS_ASSERT(stack != NULL, OS_MODULE_ID_TASK, OS_ERROR_NULLPTR);
    BEERTOS_ASSERT(priority < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);

    os_stack_t *stack_ptr = os_port_task_stack_init(task_handler, argv, stack, stack_size);

    /* Set stack pointer */
    task->sp = (void *)stack_ptr;
    task->priority = priority;
    task->ticks = 0U;

    os_tasks[priority] = task;

    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, stack, autostart, argv) \
        if (task == &name##_control) \
        { \
            BEERTOS_TRACE_TASK_CREATE(task, #name, stack); \
        }

    #define BEERTOS_TASK_TRACE_INIT() BEERTOS_TASK_LIST()
    BEERTOS_TASK_TRACE_INIT();
}

static void BeeRTOS_Idle_Task(void *argv)
{
    BEERTOS_IDLE_TASK_INIT_CB();

    while (1)
    {
        BEERTOS_IDLE_TASK_CB();
    }
}

void os_task_init(void)
{
    /* Clear all global variables */
    os_ready_mask = 0U;
    os_delay_mask = 0U;
    for (uint32_t i = 0U; i < OS_TASK_MAX; i++)
    {
        os_tasks[i] = NULL;
    }

    /* X-Macro to call os_task_create for all tasks */
    uint8_t prio = OS_TASK_MAX - 1U;
    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, stack, autostart, argv) \
        os_task_create(&name##_control, cb, name##_stack, sizeof(name##_stack), prio, argv); \
        prio--;

    #define BEERTOS_TASK_INIT_ALL() BEERTOS_TASK_LIST()

    os_task_create(&os_idle_task_control, BeeRTOS_Idle_Task, os_idle_task_stack, sizeof(os_idle_task_stack), 0U, NULL);
    BEERTOS_TRACE_TASK_CREATE(&os_idle_task_control, "IDLE", sizeof(os_idle_task_stack));

    BEERTOS_TASK_INIT_ALL();

    /* X-Macro to call os_task_start for all tasks */
    uint8_t task_id = 1U;
    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, stack, autostart, argv)    \
        if (true == autostart)                                \
        {                           \
            os_task_start(task_id); \
        }                           \
        task_id++;
    
    #define BEERTOS_TASK_START_ALL() BEERTOS_TASK_LIST()

    BEERTOS_TASK_START_ALL();
}

bool os_task_start(os_task_id_t task_id)
{
    BEERTOS_ASSERT(task_id > OS_TASK_IDLE, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task_id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task_id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);

    os_disable_all_interrupts();

    uint8_t priority = OS_TASK_MAX - task_id;

    BEERTOS_TASK_START(priority);
    BEERTOS_TASK_DELAY_CLEAR(priority);

    os_enable_all_interrupts();

    return true;
}

bool os_task_stop(os_task_id_t task_id)
{
    BEERTOS_ASSERT(task_id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(task_id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);

    os_disable_all_interrupts();

    BEERTOS_TASK_STOP(task_id);
    BEERTOS_TASK_DELAY_CLEAR(task_id);

    os_enable_all_interrupts();

    return true;
}

void os_task_release(os_task_id_t task_id)
{
    os_disable_all_interrupts();

    (void)os_task_start(task_id);
    os_sched();

    os_enable_all_interrupts();
}

void os_task_delete(void)
{
    os_disable_all_interrupts();

    os_task_stop(os_task_current->priority);
    os_tasks[os_task_current->priority] = NULL;
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
    BEERTOS_TASK_DELAY_SET(os_task_current->priority);
    BEERTOS_TASK_STOP(os_task_current->priority);
    os_sched();

    os_enable_all_interrupts();
}

void os_task_tick(void)
{
    uint32_t mask = os_delay_mask;

    while (mask)
    {
       os_task_t* const task = os_tasks[OS_GET_HIGHEST_PRIO_TASK_MASK(mask)];

        task->ticks--;
         if (task->ticks == 0)
         {
            BEERTOS_TASK_START(task->priority);
            BEERTOS_TASK_DELAY_CLEAR(task->priority);
         }
            mask &= ~(1 << (task->priority - 1U));
    }
}

void os_sched(void)
{
    os_disable_all_interrupts();

    if (os_ready_mask == 0U)
    {
        os_task_next = os_tasks[OS_TASK_IDLE];
    }
    else
    {
        os_task_next = os_tasks[OS_GET_HIGHEST_PRIO_TASK_MASK(os_ready_mask)];
    }

    if(NULL != os_task_current)
    {
        os_task_stack_mon();
    }

    if(os_task_current != os_task_next)
    {
        os_port_context_switch();
    }

    os_enable_all_interrupts();
}
