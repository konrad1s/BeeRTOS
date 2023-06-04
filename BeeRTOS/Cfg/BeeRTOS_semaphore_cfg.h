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

#define SEMAPHORE_UT_INITIAL_COUNT 10U

#define BEERTOS_SEMAPHORE_LIST               \
    /* NAME, INITIAL_COUT */                 \
    BEERTOS_SEMAPHORE(SEMAPHORE_UT,      SEMAPHORE_UT_INITIAL_COUNT) \
    BEERTOS_SEMAPHORE(SEMAPHORE_ONE,     0U) \
    BEERTOS_SEMAPHORE(SEMAPHORE_TWO,     0U)

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