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
    BEERTOS_TASK( OS_TASK_SLOW, Led_Slow_Toggle, 1, 128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_FAST, Led_Fast_Toggle, 2, 128, 1, NULL )  \
    BEERTOS_TASK( OS_TASK_UT, ut_beertos_main_task, 3, 256, 1, NULL )

#undef  BEERTOS_TASK
#define BEERTOS_TASK(task_name, task_cb, task_prio, task_stack, task_autostart, task_arg) task_name,

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

typedef enum {
    BEERTOS_TASK_LIST
} OS_TASK_ID; 

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

extern void Led_Fast_Toggle(void);
extern void Led_Slow_Toggle(void);
extern void ut_beertos_main_task(void);
extern void Idle_Task(void);

#endif /* __BEE_RTOS_TASK_CFG_H__ */