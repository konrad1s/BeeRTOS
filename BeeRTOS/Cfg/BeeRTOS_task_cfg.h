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

#define BEERTOS_TASK_LIST \
    /* task_name, callback, priority, stacksize, autostart, task_arg */ \
    BEERTOS_TASK( OS_TASK_IDLE, Idle_Task, 0, 64, 1, NULL ) \
    BEERTOS_TASK( OS_TASK_PRIO_1, ut_task_priority_1, 1, 128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_PRIO_2, ut_task_priority_2, 2, 128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_PRIO_3, ut_task_priority_3, 3, 128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_UT_MAIN, ut_beertos_main_task, 4, 256, 1, NULL )

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef  BEERTOS_TASK
#define BEERTOS_TASK(task_name, task_cb, task_prio, task_stack, task_autostart, task_arg) task_name,

typedef enum {
    BEERTOS_TASK_LIST
} OS_TASK_ID; 

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
extern void Idle_Task(void);

#endif /* __BEE_RTOS_TASK_CFG_H__ */