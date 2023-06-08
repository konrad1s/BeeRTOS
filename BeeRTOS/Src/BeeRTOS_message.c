/******************************************************************************************
 * @brief OS message source file
 * @file BeeRTOS_message.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_message.h"
#include "BeeRTOS_assert.h"
#include <string.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#undef OS_MESSAGE
#define OS_MESSAGE(name, size, count) \
    static uint8_t name ## _buffer[size * count];

#define BEERTOS_MESSAGE_BUFFERS OS_MESSAGES_LIST

BEERTOS_MESSAGE_BUFFERS

static os_message_t messages[OS_MESSAGE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
static inline void os_message_init(os_message_t *message, void *buffer, uint32_t size, uint32_t item_size)
{
    BEERTOS_ASSERT(message != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    message->buffer = buffer;
    message->size = size;
    message->item_size = item_size;
    message->head = 0U;
    message->tail = 0U;
    message->count = 0U;
}

void os_messages_init(void)
{
    /*! X-Macro to initialize all messages */
    #undef OS_MESSAGE
    #define OS_MESSAGE(name, size, count) \
        os_message_init(&messages[name], name ## _buffer, size * count, size);
    
    #define BEERTOS_MESSAGES_INIT OS_MESSAGES_LIST

    BEERTOS_MESSAGES_INIT
}

bool os_message_send(os_message_id_t id, void *data, uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX, OS_MODULE_ID_MESSAGE, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    bool ret = false;

    os_message_t *msg = &messages[id];

    os_disable_all_interrupts();

    if (msg->size > msg->head + msg->item_size)
    {
        memcpy(msg->buffer + msg->head, data, msg->item_size);
        msg->head += msg->item_size;
        msg->count++;
        ret = true;
    }
    
    os_enable_all_interrupts();

    return ret;
}

bool os_message_receive(os_message_id_t id, void *data, uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX, OS_MODULE_ID_MESSAGE, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    bool ret = false;

    os_message_t *msg = &messages[id];

    os_disable_all_interrupts();

    if (msg->count > 0U)
    {
        memcpy(data, msg->buffer + msg->tail, msg->item_size);
        msg->tail += msg->item_size;
        msg->count--;
        ret = true;
    }

    os_enable_all_interrupts();

    return ret;
}
