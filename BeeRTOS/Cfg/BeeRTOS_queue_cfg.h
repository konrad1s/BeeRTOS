/******************************************************************************************
 * @brief BeeRTOS Queue Configuration File
 * @file  BeeRTOS_queue_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_QUEUE_CFG_H__
#define __BEERTOS_QUEUE_CFG_H____

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define BEERTOS_QUEUE_LIST            \
    /* queue_id, queue size */        \
    OS_QUEUE(QUEUE_1, 10)             \
    OS_QUEUE(QUEUE_2, 10)             \

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /* __BEERTOS_QUEUE_CFG_H__ */