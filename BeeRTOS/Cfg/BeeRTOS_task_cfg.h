/******************************************************************************************
 * @brief 
 * @file 
 * ****************************************************************************************/

#ifndef __BEE_RTOS_TASK_CFG_H__
#define __BEE_RTOS_TASK_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/* Stack size of the idle task in bytes - if callbacks not used you can set 16 bytes */
#define BEERTOS_IDLE_TASK_STACK_SIZE    (32U)
/* Callback when idle task is initialized (before entering the idle loop) */
#define BEERTOS_IDLE_TASK_INIT_CB()     {}
/* Callback when idle task is running (in the idle loop) */
#define BEERTOS_IDLE_TASK_CB()          {}


/* Enable this option to use the stack overflow detection feature. 
 * When tasks is created, the stack is filled with a pattern. Each context switch
 * the pattern is checked. If the pattern is not the same, then we can assume that
 * the stack has overflowed. */
#define BEERTOS_USE_TASK_STACK_MONITOR  (true)
/* Fast metod checks only the last 4 bytes of the stack each context switch. */
#define BEERTOS_USE_FAST_STACK_MONITOR  (true)
/* This method checks the OS_TASK_STACK_CHECK_BYTE_COUNT bytes of the stack each context switch. */
#define BEERTOS_USE_USER_STACK_MONITOR  (false)
#define OS_TASK_STACK_CHECK_BYTE_COUNT  (10U)

/* Define your tasks here! */
/* In the current implementation all tasks are created statically.
 * The priority of the tasks is defined by the order of the tasks in the list -
 * the first task in the list has the highest priority.
 * You can create up to 32 tasks, but one is reserved for the idle task.
 * The structure of the task definition is as follows:
 * BEERTOS_TASK( task_name, function, stacksize, autostart, task_arg ) */
#define BEERTOS_TASK_LIST                                                           \
    BEERTOS_TASK( OS_TASK_UT_MAIN,      ut_beertos_main_task,       256, 1, NULL )  \
    /* Priority test tasks */                                                       \
    BEERTOS_TASK( OS_TASK_PRIO_3,   ut_task_priority_highest,    128, 1, NULL )     \
    BEERTOS_TASK( OS_TASK_PRIO_2,   ut_task_priority_medium,     128, 1, NULL )     \
    BEERTOS_TASK( OS_TASK_PRIO_1,   ut_task_priority_lowest,     128, 1, NULL )     \
    /* Mutex test tasks */                                                          \
    BEERTOS_TASK( OS_TASK_MUTEX_3,   ut_task_mutex_3,       128, 1, NULL )          \
    BEERTOS_TASK( OS_TASK_MUTEX_2,   ut_task_mutex_2,       128, 1, NULL )          \
    BEERTOS_TASK( OS_TASK_MUTEX_1,   ut_task_mutex_1,       128, 1, NULL )

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/
extern void ut_beertos_main_task(void *arg);

extern void ut_task_priority_lowest(void *arg);
extern void ut_task_priority_medium(void *arg);
extern void ut_task_priority_highest(void *arg);

extern void ut_task_mutex_1(void *arg);
extern void ut_task_mutex_2(void *arg);
extern void ut_task_mutex_3(void *arg);

#endif /* __BEE_RTOS_TASK_CFG_H__ */