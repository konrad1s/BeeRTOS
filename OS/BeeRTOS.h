/******************************************************************************************
 * @brief OS header file
 * @file BeeRTOS.h
 * ****************************************************************************************/

#ifndef __BEERTOS_H__
#define __BEERTOS_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include <stdint.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define OS_MAS_TASK_NB 32U

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/*! OS Thread control block */
typedef struct
{
    void *sp; /*!< stack pointer */
    uint32_t ticks; /*!< ticks */
    uint8_t priority; /*!< priority */
} os_task;

typedef void (*os_task_handler)();

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_init(void);
void os_delay(uint32_t ticks);
void os_task_delete(void);
void os_task_create(os_task *task, os_task_handler task_handler,
                    void *stack, uint32_t stack_size, uint8_t priority);
void os_run(void);

void os_disable_all_interrupts(void);
void os_enable_all_interrupts(void);

os_task* os_get_current_task(void);
void os_task_release(uint32_t task_id);

#endif /* __BEERTOS_H__ */