/******************************************************************************************
 * @brief OS semaphore header file
 * @file BeeRTOS_semaphore.h
 * ****************************************************************************************/

#ifndef __BEERTOS_SEMAPHORE_H__
#define __BEERTOS_SEMAPHORE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include <stddef.h>
#include <stdbool.h>

#include "BeeRTOS.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

typedef struct
{
    uint32_t count;
    uint32_t tasks_blocked; /* one bit represents one task */
} os_semaphore_t;


/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_semaphore_init(os_semaphore_t *semaphore, uint32_t count);
bool os_semaphore_wait(os_semaphore_t *semaphore, uint32_t timeout);
bool os_semaphore_signal(os_semaphore_t *semaphore);

#endif /* __BEERTOS_SEMAPHORE_H__ */