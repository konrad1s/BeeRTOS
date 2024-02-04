/******************************************************************************************
 * @brief Header file for BeeRTOS mutex management
 * @file BeeRTOS_mutex.h
 * This header file defines the interface for managing mutexes within BeeRTOS, facilitating
 * synchronization and mutual exclusion among tasks. It declares the enumeration for mutex
 * identifiers, based on the system configuration, and provides prototypes for functions to
 * initialize mutexes, lock and unlock them, with support for timeouts and recursive locking.
 ******************************************************************************************/

#ifndef __BEERTOS_MUTEX_H__
#define __BEERTOS_MUTEX_H__

/******************************************************************************************
*                                        INCLUDES                                        *
******************************************************************************************/

#include "Beertos_internal.h"

/******************************************************************************************
*                                         DEFINES                                        *
******************************************************************************************/

/******************************************************************************************
*                                        TYPEDEFS                                        *
******************************************************************************************/

#undef BEERTOS_MUTEX
#undef BEERTOS_TASK
#undef BEERTOS_ALARM_TASK

#define BEERTOS_MUTEX(name, initial_count) name,
#define BEERTOS_TASK(...)
#define BEERTOS_ALARM_TASK(...)

#define OS_MUTEX_LIST() BEERTOS_PRIORITY_LIST()

/*! Enumerates mutex identifiers generated from the BEERTOS_PRIORITY_LIST macro expansion,
 *  providing unique identifiers for each mutex configured in the system. */
typedef enum 
{
    OS_MUTEX_LIST()
    BEERTOS_MUTEX_ID_MAX
} os_mutex_id_t; 

/******************************************************************************************
*                                    GLOBAL VARIABLES                                    *
******************************************************************************************/

/******************************************************************************************
*                                   FUNCTION PROTOTYPES                                  *
******************************************************************************************/
void os_mutex_module_init(void);
void os_mutex_unlock(const os_mutex_id_t id);
bool os_mutex_lock(const os_mutex_id_t id, const uint32_t timeout);

#endif /* __BEERTOS_MUTEX_H__ */