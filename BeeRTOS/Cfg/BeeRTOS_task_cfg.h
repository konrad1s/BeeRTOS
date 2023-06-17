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
#define BEERTOS_IDLE_TASK_STACK_SIZE    (64U)
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

/*!
 *  @brief OS tasks configuration list - define your tasks here
 *  In the current implementation all tasks are created statically.
 *  The priority of the tasks is defined by the order of the tasks in the list -
 *  the first task in the list has the highest priority.
 *  You can create up to 32 tasks, but one is reserved for the idle task.
 * 
 *  Structure: BEERTOS_TASK(task_id, function, stacksize, autostart, task_arg)
 * @param task_id - task id (created in os_task_id_t enum), must be unique
 * @param function - task function
 * @param stacksize - task stack size in bytes
 * @param autostart - if TRUE, task will be started automatically after OS initialization
 * @param task_arg - argument passed to the task function
 * 
 * @note If you want to use the task argument, you must define the task function with void* argument
 *       and cast it to the desired type inside the function.
 *          Example: void task_function(void* arg)
 *              {
 *                  uint32_t my_arg = (uint32_t)arg;
 *              }
 */
#define BEERTOS_TASK_LIST()                                                             \
    BEERTOS_TASK( OS_TASK_UT_MAIN,      ut_beertos_main_task,       128, true, NULL )   \
    /* Priority test tasks */                                                           \
    BEERTOS_TASK( OS_TASK_PRIO_3,   ut_task_priority_highest,    128, true, NULL )      \
    BEERTOS_TASK( OS_TASK_PRIO_2,   ut_task_priority_medium,     128, true, NULL )      \
    BEERTOS_TASK( OS_TASK_PRIO_1,   ut_task_priority_lowest,     128, true, NULL )      \
    /* Mutex test tasks */                                                              \
    BEERTOS_TASK( OS_TASK_MUTEX_3,   ut_task_mutex_3,       128, true, NULL )           \
    BEERTOS_TASK( OS_TASK_MUTEX_2,   ut_task_mutex_2,       128, true, NULL )           \
    BEERTOS_TASK( OS_TASK_MUTEX_1,   ut_task_mutex_1,       128, true, NULL )           \
    /* Message test tasks */                                                            \
    BEERTOS_TASK( OS_TASK_MSG_2,   ut_task_msg_2,       128, false, NULL )              \
    BEERTOS_TASK( OS_TASK_MSG_1,   ut_task_msg_1,       128, false, NULL )


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

extern void ut_task_msg_1(void *arg);
extern void ut_task_msg_2(void *arg);

#endif /* __BEE_RTOS_TASK_CFG_H__ */