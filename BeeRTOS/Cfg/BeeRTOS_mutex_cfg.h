/******************************************************************************************
 * @brief BeeRTOS Mutex Configuration File
 * @file  BeeRTOS_semaphore_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_MUTEX_CFG_H__
#define __BEERTOS_MUTEX_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! @brief BeeRTOS mutex list - define your mutexes here
 *  Mutexes are used to protect critical sections of code from being executed by more than
 *  one task at the same time. In the current implementation, mutexes are recursive, which
 * means that the same task can lock the same mutex multiple times, but it must be unlocked
 * the same number of times.
 * Compared to semaphores, mutexes uses priority inheritance, which means that if a task
 * with higher priority is blocked on a mutex, the priority of the task that owns the mutex
 * will be raised to the priority of the blocked task. This prevents priority inversion.
 * 
 *  Structure: BEERTOS_MUTEX(mutex_id, initial_count)
 * @param mutex_id - mutex id (created in os_mutex_id_t enum), must be unique
 * @param initial_count - initial count of the mutex
 */
#define BEERTOS_MUTEX_LIST()        \
    BEERTOS_MUTEX(MUTEX_ONE,    0U) \
    BEERTOS_MUTEX(MUTEX_TWO,    0U) \
    BEERTOS_MUTEX(MUTEX_THREE,  0U)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /* __BEERTOS_MUTEX_CFG_H__ */