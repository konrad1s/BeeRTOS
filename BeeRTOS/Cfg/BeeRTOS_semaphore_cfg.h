/******************************************************************************************
 * @brief BeeRTOS Semaphore Configuration File
 * @file  BeeRTOS_semaphore_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_SEMAPHORE_CFG_H__
#define __BEERTOS_SEMAPHORE_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define SEMAPHORE_UT1_INITIAL_COUNT (10U)

/*! @brief BeeRTOS semaphore list - define your semaphores here
 * Semaphores are used to synchronize tasks and to protect shared resources from being
 * accessed by more than one task at the same time. Semaphores can be used to implement
 * mutual exclusion, but mutexes are preferred for this purpose. Semaphores do not implement
 * priority inheritance. Semaphores can be used for signaling between tasks.
 * 
 * Structure: BEERTOS_SEMAPHORE(semaphore_id, initial_count, type)
 * @param semaphore_id - semaphore id (created in os_semaphore_id_t enum), must be unique
 * @param initial_count - initial count of the semaphore
 * @param type - semaphore type (counting or binary)
 */
#define BEERTOS_SEMAPHORE_LIST()               \
    BEERTOS_SEMAPHORE(SEMAPHORE_UT1,     SEMAPHORE_UT1_INITIAL_COUNT,   SEMAPHORE_TYPE_COUNTING) \
    BEERTOS_SEMAPHORE(SEMAPHORE_UT2,     0U,                            SEMAPHORE_TYPE_BINARY)   \
    BEERTOS_SEMAPHORE(SEMAPHORE_TWO,     0U,                            SEMAPHORE_TYPE_BINARY)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/ 

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /* __BEERTOS_SEMAPHORE_CFG_H__ */