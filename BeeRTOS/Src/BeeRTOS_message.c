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

/*! Structure describing a message (created using os_queue_t structure) */
typedef struct
{
    os_queue_t *queue;
    uint32_t item_size;
    os_task_mask_t tasks_blocked;
} os_message_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_queue_t os_queues[OS_MSG_QUEUE_ID_MAX];
extern os_task_t *os_tasks[OS_TASK_MAX];

/*! List of all messages */
static os_message_t os_messages[OS_MESSAGE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

void os_message_init(void)
{
    /*! X-Macro to initialize all messages */
    #undef OS_MESSAGE
    #define OS_MESSAGE(name, count, size)                                  \
        os_messages[name].queue = &os_queues[name + BEERTOS_QUEUE_ID_MAX]; \
        os_messages[name].item_size = size;                                \
        os_messages[name].tasks_blocked = 0U;

    #define BEERTOS_MESSAGES_INIT_ALL() OS_MESSAGES_LIST()

    BEERTOS_MESSAGES_INIT_ALL();
}

bool os_message_send(os_message_id_t id, void *data, uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX, OS_MODULE_ID_MESSAGE, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_MESSAGE, OS_ERROR_NULLPTR);

    bool ret = false;
    os_message_t *const msg = &os_messages[id];

    /* Check if message can be pushed instantly */
    if (true == os_queue_push(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
        ret = true;
    }
    else
    {
        /* If timeout is set, wait for the other task to pop the message */
        if (0U != timeout)
        {
            msg->tasks_blocked |= (1U << (os_get_current_task()->priority - 1U));
            os_delay(timeout);

            /* Try to push the message again, if it fails, it means that the timeout expired
               and the message was not popped by the other task */
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

    os_message_t *const msg = &os_messages[id];

    // os_disable_all_interrupts();

    /* Check if message can be popped instantly */
    if (true == os_queue_pop(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
        ret = true;

        /* If there are tasks blocked on this message, release the highest priority one */
        if (0U != msg->tasks_blocked)
        {
            os_task_t *task = os_tasks[OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(msg->tasks_blocked)];
            msg->tasks_blocked &= ~(1U << (task->priority - 1U));
            os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
        }
    }

    // os_enable_all_interrupts();

    return ret;
}
