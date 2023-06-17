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

/*! @brief BeeRTOS queue list - define your queues here
 *  Queues are more general than messages, they can store any type of data
 *  There can be pushed any number of elements to queue, as long as queue is not full.
 *  There can be popped any number of elements from queue, as long as queue is not empty.
 *  Queues in BeeRTOS are implemented as circular buffers, if queue is full, new elements
 *  will not overwrite old ones, but will be discarded without task delay.
 * 
 *  Structure: OS_QUEUE(queue_id, queue size)
 * @param queue_id - queue id (created in os_queue_id_t enum), must be unique
 * @param queue size - queue size in bytes - maximum number of elements that can be stored in queue
 */
#define BEERTOS_QUEUE_LIST()          \
    OS_QUEUE(QUEUE_1, 10U)            \
    OS_QUEUE(QUEUE_2, 10U)            \

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