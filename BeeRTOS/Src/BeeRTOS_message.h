/******************************************************************************************
 * @brief OS message header file
 * @file BeeRTOS_message.h
 * ****************************************************************************************/

#ifndef __BEERTOS_MESSAGE_H__
#define __BEERTOS_MESSAGE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"
#include "BeeRTOS_message_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef OS_MESSAGE
#define OS_MESSAGE(name, ...) name,

typedef enum
{
    OS_MESSAGES_LIST
    OS_MESSAGE_ID_MAX
} os_message_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_messages_init(void);
bool os_message_send(os_message_id_t id, void *data, uint32_t timeout);
bool os_message_receive(os_message_id_t id, void *data, uint32_t timeout);

#endif /* __BEERTOS_MESSAGE_H__ */