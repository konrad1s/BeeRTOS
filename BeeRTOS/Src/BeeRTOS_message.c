/******************************************************************************************
 * @brief OS message source file
 * @file BeeRTOS_message.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_message.h"
#include "BeeRTOS_assert.h"
#include "BeeRTOS_task.h"
#include <string.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

typedef struct
{
    void *buffer;
    uint32_t size;
    uint32_t item_size;
    uint32_t head;
    uint32_t tail;
    uint32_t count;

    uint32_t tasks_blocked;
} os_message_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_task_t *os_tasks[OS_TASK_MAX];

#undef OS_MESSAGE
#define OS_MESSAGE(name, size, count) \
    static uint8_t name ## _buffer[size * count];

#define BEERTOS_MESSAGE_BUFFERS OS_MESSAGES_LIST

BEERTOS_MESSAGE_BUFFERS

static os_message_t messages[OS_MESSAGE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
static inline void os_message_init(os_message_t *msg, void *buffer, uint32_t size, uint32_t item_size)
{
    BEERTOS_ASSERT(msg != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    msg->buffer = buffer;
    msg->size = size;
    msg->item_size = item_size;
    msg->head = 0U;
    msg->tail = 0U;
    msg->count = 0U;
    msg->tasks_blocked = 0U;
}

static inline bool os_push_into_queue(os_message_t *msg, void *data)
{
    os_disable_all_interrupts();

    bool ret = false;
    if (msg->size >= msg->head + msg->item_size)
    {
        memcpy(msg->buffer + msg->head, data, msg->item_size);
        msg->head += msg->item_size;
        msg->count++;
        ret = true;
    }

    os_enable_all_interrupts();

    return ret;
}

void os_messages_init(void)
{
    /*! X-Macro to initialize all messages */
    #undef OS_MESSAGE
    #define OS_MESSAGE(name, count, size) \
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

    if (true == os_push_into_queue(msg, data))
    {
        ret = true;
    }
    else
    {
        if (0U != timeout)
        {
            msg->tasks_blocked |= (1U << (os_get_current_task()->priority - 1U));
            os_delay(timeout);

            if (true == os_push_into_queue(msg, data))
            {
                ret = true;
            }
        }
    }

    return ret;
}

bool os_message_receive(os_message_id_t id, void *data, uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX, OS_MODULE_ID_MESSAGE, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    bool ret = false;

    os_message_t *msg = &messages[id];

    // os_disable_all_interrupts();

    if (msg->count > 0U)
    {
        memcpy(data, msg->buffer + msg->tail, msg->item_size);
        msg->tail += msg->item_size;
        msg->count--;
        ret = true;

        if (0U != msg->tasks_blocked)
        {
            os_task_t* task = os_tasks[OS_LOG2(msg->tasks_blocked) & 0xFFU];
            msg->tasks_blocked &= ~(1U << (task->priority - 1U));
            os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
        }
    }

    // os_enable_all_interrupts();

    return ret;
}
