/******************************************************************************************
* @brief OS semaphore header file
* @file BeeRTOS_semaphore.h
* ****************************************************************************************/

#ifndef __BEERTOS_SEMAPHORE_H__
#define __BEERTOS_SEMAPHORE_H__

/******************************************************************************************
*                                        INCLUDES                                        *
******************************************************************************************/

#include "Beertos_internal.h"
#include "BeeRTOS_semaphore_cfg.h"

/******************************************************************************************
*                                         DEFINES                                        *
******************************************************************************************/

/******************************************************************************************
*                                        TYPEDEFS                                        *
******************************************************************************************/

#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, initial_count) name,
typedef enum 
{
    BEERTOS_SEMAPHORE_LIST
    BEERTOS_SEMAPHORE_ID_MAX
} os_sem_id_t;

typedef struct
{
    uint32_t count;
    uint32_t tasks_blocked; /* one bit represents one task */
} os_sem_t;

/******************************************************************************************
*                                    GLOBAL VARIABLES                                    *
******************************************************************************************/

/******************************************************************************************
*                                   FUNCTION PROTOTYPES                                  *
******************************************************************************************/

void os_semaphores_init(void);
bool os_semaphore_wait(os_sem_id_t id, uint32_t timeout);
bool os_semaphore_signal(os_sem_id_t id);

#if (BEERTOS_USE_GET_CONTROL_BLOCK_API == true)
bool os_semaphore_get_control_block_info(os_sem_id_t id, os_sem_t **sem);
#endif

#endif /* __BEERTOS_SEMAPHORE_H__ */