/******************************************************************************************
* @brief OS mutex header file
* @file BeeRTOS_mutex.h
* ****************************************************************************************/

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
#define BEERTOS_MUTEX(name, initial_count) name,
#undef BEERTOS_TASK
#define BEERTOS_TASK(...)
#undef BEERTOS_ALARM_TASK
#define BEERTOS_ALARM_TASK(...)

#define BEERTOS_MUTEX_LIST() BEERTOS_PRIORITY_LIST()

/*! Mutex IDs - generated from BEERTOS_MUTEX_LIST() in BeeRTOS_mutex_cfg.h */
typedef enum 
{
    BEERTOS_MUTEX_LIST()
    BEERTOS_MUTEX_ID_MAX
} os_mutex_id_t; 

/******************************************************************************************
*                                    GLOBAL VARIABLES                                    *
******************************************************************************************/

/******************************************************************************************
*                                   FUNCTION PROTOTYPES                                  *
******************************************************************************************/

/**
 * @brief This function initializes all mutexes.
 * 
 * @return None
 */
void os_mutex_module_init(void);

/**
 * @brief This function unlocks the specified mutex. Can be called only if the mutex is locked.
 * If the mutex is locked by another task, the function will rise an error. If the mutex was
 * locked multiple times, it must be unlocked the same number of times.
 *
 * @param id - mutex id
 * @return None
 */
void os_mutex_unlock(const os_mutex_id_t id);

/**
 * @brief This function locks the specified mutex. If the mutex is already locked, the function
 * will wait until the mutex is unlocked (with timeout). If the mutex is already locked by the
 * calling task, recursive locking is performed. The mutex must be unlocked as many times as it
 * was locked.
 * 
 * @param id - mutex id
 * @param timeout - maximum time to wait for the mutex to be unlocked. If timeout is 0, the function
 *                  will return immediately false if the mutex is locked.
 */
bool os_mutex_lock(const os_mutex_id_t id, const uint32_t timeout);

#endif /* __BEERTOS_MUTEX_H__ */