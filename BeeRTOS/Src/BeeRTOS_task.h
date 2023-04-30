/******************************************************************************************
 * @brief OS task header file
 * @file BeeRTOS_task.h
 * ****************************************************************************************/

#ifndef __BEERTOS_TASK_H__
#define __BEERTOS_TASK_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include <stdbool.h>
#include "BeeRTOS_task_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define OS_MAS_TASK_NB (32U)

#define OS_TASK_STACK_PATTERN ((os_stack_t)0xA5A5A5A5U)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/*! OS Thread control block */
typedef struct
{
    volatile void *sp; /*!< stack pointer */
    uint32_t ticks; /*!< ticks */
    uint8_t priority; /*!< priority */
} os_task_t;

typedef void (*os_task_handler)();

#undef  BEERTOS_TASK
#define BEERTOS_TASK(task_name, task_cb, task_stack, task_autostart, task_arg) task_name,
typedef enum {
    OS_TASK_IDLE = 0, /* Reserved for idle task! */
    BEERTOS_TASK_LIST
    OS_TASK_MAX
} os_task_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/
void os_task_init(void);
bool os_task_start(os_task_id_t task_id);
bool os_task_stop(os_task_id_t task_id);
void os_task_delete(void);
void os_task_release(uint32_t task_id);
os_task_t* os_get_current_task(void);
void os_delay(uint32_t ticks);
void os_task_tick(void);

#endif /* __BEERTOS_TASK_H__ */