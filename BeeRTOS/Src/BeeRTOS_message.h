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
void os_message_module_init(void);
bool os_message_send(const os_message_id_t id, const void *const data, const uint32_t timeout);
bool os_message_receive(const os_message_id_t id, void *const data, const uint32_t timeout);

#endif /* __BEERTOS_MESSAGE_H__ */