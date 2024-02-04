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

#define BIT_SET(var, bit) (var |= (1U << bit))
#define BIT_CLEAR(var, bit) (var &= ~(1U << bit))

#define BEERTOS_TASK_START(task_id) (BIT_SET(os_ready_mask, (task_id - 1U)))
#define BEERTOS_TASK_STOP(task_id) (BIT_CLEAR(os_ready_mask, (task_id - 1U)))

#define BEERTOS_TASK_DELAY_SET(task_id) (BIT_SET(os_delay_mask, (task_id - 1U)))
#define BEERTOS_TASK_DELAY_CLEAR(task_id) (BIT_CLEAR(os_delay_mask, (task_id - 1U)))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#undef BEERTOS_TASK
#undef BEERTOS_MUTEX
#undef BEERTOS_ALARM_TASK

/*! X-Macro to create task stack array for all tasks and alarm tasks */
#define BEERTOS_MUTEX(...)
#define BEERTOS_TASK(name, cb, stack, autostart, argv) \
    static os_stack_t name##_stack[stack];
#define BEERTOS_ALARM_TASK(name, stack) \
    static os_stack_t name##_stack[stack];

/* This macro creates the stack arrays for all tasks */
#define OS_CREATE_STACK_VAR() BEERTOS_PRIORITY_LIST()

static os_stack_t os_idle_task_stack[BEERTOS_IDLE_TASK_STACK_SIZE];
OS_CREATE_STACK_VAR();

/* ****************************************************************************************** */

#undef BEERTOS_TASK
#undef BEERTOS_MUTEX
#undef BEERTOS_ALARM_TASK

/*! X-Macro to create array of pointers to stack arrays for all tasks */
#define BEERTOS_MUTEX(...) \
    NULL,
#define BEERTOS_TASK(name, ...) \
    name##_stack,
#define BEERTOS_ALARM_TASK(name, stack) \
    name##_stack,

/* This macro creates the array of pointers to stack arrays for all tasks */
#define OS_CREATE_STACK_ARRAY() BEERTOS_PRIORITY_LIST()

static os_stack_t *os_task_stacks[] =
{
    os_idle_task_stack,
    OS_CREATE_STACK_ARRAY()
};

/* ****************************************************************************************** */

#undef BEERTOS_TASK
#undef BEERTOS_MUTEX
#undef BEERTOS_ALARM_TASK

/*! X-Macro to create task control structure for all tasks */
#define BEERTOS_MUTEX(...)
#define BEERTOS_TASK(name, ...) \
    static os_task_t name##_control;
#define BEERTOS_ALARM_TASK(name, ...) \
    static os_task_t name##_control;

/* This macro creates the task control structure for all tasks */
#define OS_CREATE_TASK_CONTROL_BLOCK() BEERTOS_PRIORITY_LIST()

static os_task_t os_idle_task_control;
OS_CREATE_TASK_CONTROL_BLOCK();

/* ****************************************************************************************** */

/*! Pointer to the current task */
os_task_t *volatile os_task_current;

/*! Pointer to the next task */
os_task_t *volatile os_task_next;

/*! Array of pointers to task control structures, the index is the priority of the task */
os_task_t *os_tasks[OS_TASK_MAX];

/*! Mask of ready tasks */
static os_task_mask_t os_ready_mask;
/*! Mask of delayed tasks */
static os_task_mask_t os_delay_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

extern os_stack_t *os_port_task_stack_init(void (*task)(void *), void *arg, void *stack_ptr, uint32_t stack_size);
extern void os_port_context_switch(void);

#if (BEERTOS_USE_TASK_STACK_MONITOR == true)
#if (BEERTOS_USE_FAST_STACK_MONITOR == true)
static inline void os_task_stack_mon(void)
{
    const os_task_t *const task = os_task_current;
    const os_stack_t pattern = OS_TASK_STACK_PATTERN;

    if ((os_task_stacks[OS_GET_TASK_ID_FROM_PRIORITY(task->priority)][0] != pattern) ||
        (os_task_stacks[OS_GET_TASK_ID_FROM_PRIORITY(task->priority)][1] != pattern) ||
        (os_task_stacks[OS_GET_TASK_ID_FROM_PRIORITY(task->priority)][2] != pattern) ||
        (os_task_stacks[OS_GET_TASK_ID_FROM_PRIORITY(task->priority)][3] != pattern))
    {
        /* TODO - handle stack overflow */
        // while (1)
        // ;
    }
}
#elif (BEERTOS_USE_USER_STACK_MONITOR == true)
static inline void os_task_stack_mon(void)
{
    const os_task_t *const task = os_task_current;
    const os_stack_t pattern = OS_TASK_STACK_PATTERN;

    for (uint32_t i = 0U; i < OS_TASK_STACK_CHECK_BYTE_COUNT; i++)
    {
        if (os_task_stacks[task->priority][i] != pattern)
        {
            /* TODO - handle stack overflow */
            while (1)
                ;
        }
    }
}
#endif /* BEERTOS_USE_USER_STACK_MONITOR */
#endif /* BEERTOS_USE_TASK_STACK_MONITOR */

static void os_task_create(os_task_t *const task,
                           const os_task_handler task_handler,
                           void *const stack,
                           const uint32_t stack_size,
                           const uint8_t priority,
                           const void *const argv)
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
}

static void os_idle_task(void *argv)
{
    BEERTOS_IDLE_TASK_INIT_CB();

    while (1)
    {
        BEERTOS_IDLE_TASK_CB();
    }
}

static void os_tasks_init(void)
{
    /* Start from the max priority, since BEERTOS_PRIORITY_LIST 
       is defined from the highest priority to the lowest */
    uint8_t priority = OS_TASK_MAX - 1U;

    #undef BEERTOS_TASK
    #undef BEERTOS_MUTEX
    #undef BEERTOS_ALARM_TASK

    /* X-Macro to call os_task_create for all tasks */
    #define BEERTOS_TASK(name, cb, stack, autostart, argv)          \
        os_task_create(&name##_control, cb, name##_stack,           \
                       sizeof(name##_stack), priority, argv);       \
        BEERTOS_TRACE_TASK_CREATE(&name##_control, #name, stack);   \
        priority--;

    #define BEERTOS_ALARM_TASK(name, stack)                          \
        os_task_create(&name##_control, os_alarm_task, name##_stack, \
                       sizeof(name##_stack), priority, NULL);        \
        BEERTOS_TRACE_TASK_CREATE(&name##_control, #name, stack);    \
        priority--;

    #define BEERTOS_MUTEX(name, ...)   \
        os_tasks[priority] = NULL;     \
        priority--;

    /* This macro calls os_task_create with the correct priority for all tasks */
    #define OS_TASK_INIT_ALL() BEERTOS_PRIORITY_LIST()
    OS_TASK_INIT_ALL();

    /* Idle task must be always created */
    os_task_create(&os_idle_task_control, os_idle_task, os_idle_task_stack,
                   sizeof(os_idle_task_stack), 0U, NULL);
    BEERTOS_TRACE_TASK_CREATE(&os_idle_task_control, "OS_TASK_IDLE",
                              sizeof(os_idle_task_stack));
}

static void os_tasks_start(void)
{
    /* Start from 1, since OS_TASK_IDLE is already started */
    uint8_t task_id = 1U;
    #undef BEERTOS_TASK
    #undef BEERTOS_MUTEX
    #undef BEERTOS_ALARM_TASK

    /* X-Macro to call os_task_start if autostart is true */
    #define BEERTOS_TASK(name, cb, stack, autostart, argv) \
        if (autostart)                                     \
        {                                                  \
            os_task_start(task_id);                        \
        }                                                  \
        task_id++;

    #define BEERTOS_MUTEX(...) \
        task_id++;

    #define BEERTOS_ALARM_TASK(...) \
        task_id++;

    #define OS_TASK_START_ALL() BEERTOS_PRIORITY_LIST()
    OS_TASK_START_ALL();
}

void os_task_module_init(void)
{
    os_ready_mask = 0U;
    os_delay_mask = 0U;
    os_tasks_init();
    os_tasks_start();
}

bool os_task_start(const os_task_id_t id)
{
    BEERTOS_ASSERT(id > OS_TASK_IDLE, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);

    const uint8_t priority = OS_TASK_MAX - id;

    os_enter_critical_section();

    BEERTOS_TASK_START(priority);
    BEERTOS_TASK_DELAY_CLEAR(priority);
    BEERTOS_TRACE_TASK_READY(os_tasks[priority]);

    os_leave_critical_section();

    return true;
}

bool os_task_stop(const os_task_id_t id)
{
    BEERTOS_ASSERT(id < OS_TASK_MAX, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(id < 255U, OS_MODULE_ID_TASK, OS_ERROR_INVALID_PARAM);

    const uint8_t priority = OS_TASK_MAX - id;

    os_enter_critical_section();

    BEERTOS_TASK_STOP(priority);
    BEERTOS_TASK_DELAY_CLEAR(priority);

    os_leave_critical_section();

    return true;
}

void os_task_release(const os_task_id_t id)
{
    os_enter_critical_section();

    (void)os_task_start(id);
    os_sched();

    os_leave_critical_section();
}

void os_task_delete(void)
{
    os_enter_critical_section();

    os_task_stop(OS_GET_TASK_ID_FROM_PRIORITY(os_task_current->priority));
    os_tasks[os_task_current->priority] = NULL;
    os_sched();

    os_leave_critical_section();
}

void os_delay(const uint32_t ticks)
{
    os_enter_critical_section();

    os_task_current->ticks = ticks;
    BEERTOS_TASK_DELAY_SET(os_task_current->priority);
    BEERTOS_TASK_STOP(os_task_current->priority);
    BEERTOS_TRACE_TASK_DELAYED(os_task_current);

    os_sched();

    os_leave_critical_section();
}

void os_task_tick(void)
{
    uint32_t mask = os_delay_mask;

    /* Decrement ticks for all delayed tasks */
    while (mask)
    {
        os_task_t *const task = os_tasks[OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask)];

        task->ticks--;
        if (task->ticks == 0)
        {
            /* Task is ready to run */
            BEERTOS_TASK_START(task->priority);
            BEERTOS_TASK_DELAY_CLEAR(task->priority);
            BEERTOS_TRACE_TASK_READY(task);
        }
        /* Clear the delay bit from the mask */
        mask &= ~(1 << (task->priority - 1U));
    }
}

void os_sched(void)
{
    os_enter_critical_section();

    if (os_ready_mask == 0U)
    {
        /* No task is ready to run, run the idle task */
        os_task_next = os_tasks[OS_TASK_IDLE];
    }
    else
    {
        /* Get the task with the highest priority */
        os_task_next = os_tasks[OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(os_ready_mask)];
    }

    if (NULL != os_task_current)
    {
        /* Stack monitoring */
        os_task_stack_mon();
    }

    /* Context switch if the next task is different from the current task */
    if (os_task_current != os_task_next)
    {
        os_port_context_switch();
    }

    os_leave_critical_section();
}
