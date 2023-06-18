/******************************************************************************************
 * @brief BeeRTOS Message Configuration File
 * @file  BeeRTOS_message_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_MESSAGE_CFG_H__
#define __BEERTOS_MESSAGE_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! @brief BeeRTOS message list - define your messages here
 * Messages are more specific than queues, they can store only one type of data
 * Messages in BeeRTOS are implemented as circular buffers, if message is full, new elements
 * will not overwrite old ones, task will be delayed until there is space in message, or
 * timeout occurs.
 *
 * Structure: OS_MESSAGE(message_id, messages_count, message_size)
 * @param message_id - message id (created in os_message_id_t enum), must be unique
 * @param messages_count - number of messages that can be stored in queue
 * @param message_size - size of single message in bytes
 */
#define OS_MESSAGES_LIST()              \
    OS_MESSAGE(MESSAGE_ONE,   2,  8)    \
    OS_MESSAGE(MESSAGE_TWO,   10, 4)    \
    OS_MESSAGE(MESSAGE_THREE, 10, 4)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /* __BEERTOS_MESSAGE_CFG_H__ */