/******************************************************************************************
 * @brief OS message header file for BeeRTOS
 * Provides the interface for message queue operations including initialization, sending,
 * and receiving messages within the BeeRTOS operating system. This header defines the
 * necessary data types and function prototypes.
 * 
 * @file BeeRTOS_message.h
 ******************************************************************************************/

#ifndef __BEERTOS_MESSAGE_H__
#define __BEERTOS_MESSAGE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#if ((BEERTOS_QUEUE_MODULE_EN != true) && (BEERTOS_MESSAGE_MODULE_EN == true))
    #error "Message module requires queue module to be enabled"
#endif

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef OS_MESSAGE
#define OS_MESSAGE(name, ...) name,

/*! Message IDs - generated from OS_MESSAGES_LIST() in BeeRTOS_message_cfg.h */
typedef enum
{
    OS_MESSAGES_LIST()
    OS_MESSAGE_ID_MAX
} os_message_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

/**
 * @brief The function initializes the operating system's messages.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 *
 * @param None
 * @return None
 */
void os_message_module_init(void);

/**
 * @brief This function sends a message to the specified message queue.
 * 
 * @param id - message queue id
 * @param data - pointer to the data to be sent
 * @param timeout - maximum time to wait for the message to be sent. If timeout is 0, the function
 *                  will return immediately false if the message queue is full.
 * @return true - message sent successfully in the given timeout
 *         false - message not sent
 */
bool os_message_send(const os_message_id_t id, const void *const data, const uint32_t timeout);

/**
 * @brief This function receives a message from the specified message queue.
 * 
 * @param id - message queue id
 * @param data - pointer to the data to be received
 * @param timeout - maximum time to wait for the message to be received. If timeout is 0, the function
 *                  will return immediately false if the message queue is empty.
 * @return true - message received successfully in the given timeout
 *         false - message not received
 */
bool os_message_receive(const os_message_id_t id, void *const data, const uint32_t timeout);

#endif /* __BEERTOS_MESSAGE_H__ */