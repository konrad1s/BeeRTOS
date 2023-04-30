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

#define BEERTOS_IDLE_TASK_STACK_SIZE    (64U)
#define BEERTOS_IDLE_TASK_INIT()        {}
#define BEERTOS_IDLE_TASK()             {}


#define BEERTOS_USE_TASK_STACK_MONITOR    (true)

/* Define your tasks here! */
/* In the current implementation all tasks are created statically.
 * The priority of the tasks is defined by the order of the tasks in the list -
 * the first task in the list has the highest priority.
 * You can create up to 32 tasks, but one is reserved for the idle task.
 * The structure of the task definition is as follows:
 * BEERTOS_TASK( task_name, function, stacksize, autostart, task_arg ) */
#define BEERTOS_TASK_LIST                                                   \
    BEERTOS_TASK( OS_TASK_UT_MAIN,  ut_beertos_main_task,   256, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_PRIO_3,   ut_task_priority_3,     128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_PRIO_2,   ut_task_priority_2,     128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_PRIO_1,   ut_task_priority_1,     128, 1, NULL )

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/
extern void ut_task_priority_1(void);
extern void ut_task_priority_2(void);
extern void ut_task_priority_3(void);
extern void ut_beertos_main_task(void);

#endif /* __BEE_RTOS_TASK_CFG_H__ */