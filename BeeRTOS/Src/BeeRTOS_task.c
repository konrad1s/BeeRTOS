/******************************************************************************************
 * @brief OS task source file
 * @file BeeRTOS_task.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS.h"
#include "BeeRTOS_task_cfg.h"
#include <stdbool.h>

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
    static uint32_t name ## _stack[stack];

#define BEERTOS_STACK_VAR BEERTOS_TASK_LIST

BEERTOS_STACK_VAR;

/*! X-Macro to create task control structure for all tasks */
#undef BEERTOS_TASK
#define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
    static os_task_t name ## _control;

#define BEERTOS_TASK_CONTROL_VAR BEERTOS_TASK_LIST

BEERTOS_TASK_CONTROL_VAR;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_task_init_all(void)
{
    /*! X-Macro to call os_task_create for all tasks */
    #undef BEERTOS_TASK
    #define BEERTOS_TASK(name, cb, prio, stack, autostart, argv) \
        os_task_create(&name##_control, cb, name##_stack, sizeof(name##_stack), prio);

    #define BEERTOS_TASK_INIT BEERTOS_TASK_LIST

    BEERTOS_TASK_INIT;
}

bool os_task_start(os_task_id_t task_id)
{
    /* TODO */
}

bool os_task_stop(os_task_id_t task_id)
{
    /* TODO */
}
