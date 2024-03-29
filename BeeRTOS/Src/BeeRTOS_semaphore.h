/******************************************************************************************
 * @brief Header file for BeeRTOS semaphore management
 * @file BeeRTOS_semaphore.h
 * This header file defines the interface for managing semaphores within BeeRTOS, facilitating
 * synchronization and coordination among tasks. It declares the enumeration for semaphore
 * identifiers, based on the system configuration, and provides prototypes for functions to
 * initialize semaphores, wait on (acquire) semaphores with optional timeout, and signal
 * (release) semaphores. Supports both binary and counting semaphores, configurable through
 * the system configuration.
 ******************************************************************************************/

#ifndef __BEERTOS_SEMAPHORE_H__
#define __BEERTOS_SEMAPHORE_H__

/******************************************************************************************
*                                        INCLUDES                                        *
******************************************************************************************/

#include "Beertos_internal.h"

/******************************************************************************************
*                                         DEFINES                                        *
******************************************************************************************/

/* Semaphore types */
#define SEMAPHORE_TYPE_BINARY       (0U)
#define SEMAPHORE_TYPE_COUNTING     (1U)

/******************************************************************************************
*                                        TYPEDEFS                                        *
******************************************************************************************/

#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, ...) name,
typedef enum 
{
    BEERTOS_SEMAPHORE_LIST()
    BEERTOS_SEMAPHORE_ID_MAX
} os_sem_id_t;


/* Counting semaphores are used if at least one counting semaphore is defined.
 * The magic code below sums up all the types of semaphores and if the sum is greater than 0,
 * counting semaphores are used (SEMAPHORE_TYPE_COUNTING is equal to 1U). */
#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, initial_count, type) + type

#if (BEERTOS_SEMAPHORE_LIST() > 0U)
#define BEERTOS_SEMAPHORE_COUNTING_USED     (true)
#else
#define BEERTOS_SEMAPHORE_COUNTING_USED     (false)
#endif


/******************************************************************************************
*                                    GLOBAL VARIABLES                                    *
******************************************************************************************/

/******************************************************************************************
*                                   FUNCTION PROTOTYPES                                  *
******************************************************************************************/
void os_semaphore_module_init(void);
bool os_semaphore_wait(const os_sem_id_t id, const uint32_t timeout);
bool os_semaphore_signal(const os_sem_id_t id);

#endif /* __BEERTOS_SEMAPHORE_H__ */