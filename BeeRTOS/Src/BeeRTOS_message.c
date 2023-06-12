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
#include "BeeRTOS_queue.h"
#include <string.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

typedef struct
{
    os_queue_t *queue;
    uint32_t item_size;
    uint32_t tasks_blocked;
} os_message_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/
extern os_queue_t os_queues[OS_MSG_QUEUE_ID_MAX];
extern os_task_t *os_tasks[OS_TASK_MAX];

static os_message_t messages[OS_MESSAGE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

void os_messages_init(void)
{
    /*! X-Macro to initialize all messages */
    #undef OS_MESSAGE
    #define OS_MESSAGE(name, count, size) \
        messages[name].queue = &os_queues[name + BEERTOS_QUEUE_ID_MAX]; \
        messages[name].item_size = size; \
        messages[name].tasks_blocked = 0U;
    
    #define BEERTOS_MESSAGES_INIT OS_MESSAGES_LIST

    BEERTOS_MESSAGES_INIT
}

bool os_message_send(os_message_id_t id, void *data, uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX, OS_MODULE_ID_MESSAGE, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    bool ret = false;

    os_message_t *msg = &messages[id];

    if (true == os_queue_push(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
        ret = true;
    }
    else
    {
        if (0U != timeout)
        {
            msg->tasks_blocked |= (1U << (os_get_current_task()->priority - 1U));
            os_delay(timeout);

            if (true == os_queue_push(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
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

    if (true == os_queue_pop(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
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
