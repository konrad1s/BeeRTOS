/******************************************************************************************
 * @brief Source file for BeeRTOS message management
 * @file BeeRTOS_message.c
 * This file implements the core functionality for message-based inter-task communication within
 * the BeeRTOS operating system. It defines operations for initializing message queues, sending
 * messages between tasks, and receiving messages. These operations allow for efficient and
 * thread-safe communication across different parts of the system, leveraging the underlying
 * queue management mechanisms.
 ******************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_message.h"
#include "BeeRTOS_assert.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_queue.h"
#include "BeeRTOS_trace_cfg.h"
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
    os_task_mask_t send_waiting_tasks;
    os_task_mask_t receive_waiting_tasks;
} os_message_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_queue_t os_queues[OS_MSG_QUEUE_ID_MAX];
extern os_task_t *os_tasks[OS_TASK_MAX];
extern os_task_t *volatile os_task_current;

/*! List of all messages */
static os_message_t os_messages[OS_MESSAGE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

static void os_message_release_waiting_task(os_task_mask_t *const task_mask)
{
    const os_task_id_t highest_prio_task = OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(*task_mask);
    os_task_t *const task = os_tasks[highest_prio_task];

    /* Clear the bit in the mask */
    *task_mask &= ~(1U << (task->priority - 1U));
    /* Release the task */
    os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
    BEERTOS_TRACE_MESSAGE_UNBLOCKED(task);
}

static bool os_message_handle_timeout(os_message_t *msg,
                                      const void *data,
                                      const uint32_t timeout,
                                      const bool is_send_operation)
{
    os_task_mask_t *waiting_tasks = is_send_operation ? &msg->send_waiting_tasks : &msg->receive_waiting_tasks;
    bool (*queue_op)(uint32_t, const void *, uint32_t) = is_send_operation ? os_queue_push : os_queue_pop;
    const uint32_t id = msg - os_messages;
    const uint8_t current_task_priority = os_task_current->priority;

    /* Mark the task as waiting */
    *waiting_tasks |= (1U << (current_task_priority - 1U));

    os_delay(timeout);
    BEERTOS_TRACE_MESSAGE_BLOCKED(os_task_current);
    /* Scheduler will be called by the delay function, enable interrupts to
       allow possible context switch */
    os_leave_critical_section();

    /* End of the delay, disable interrupts again */
    os_enter_critical_section();

    /* Try to perform the operation again */
    bool operation_success = queue_op(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size);

    /* Clear the waiting bit */
    *waiting_tasks &= ~(1U << (current_task_priority - 1U));

    return operation_success;
}

/**
 * @brief The function initializes the operating system's messages.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 *
 * @param None
 * @return None
 */
void os_message_module_init(void)
{
    /*! X-Macro to initialize all messages */
    #undef OS_MESSAGE
    #define OS_MESSAGE(name, count, size)                                  \
        os_messages[name].queue = &os_queues[name + BEERTOS_QUEUE_ID_MAX]; \
        os_messages[name].item_size = size;                                \
        os_messages[name].send_waiting_tasks = 0U;

    #define OS_MESSAGE_INIT_ALL() OS_MESSAGES_LIST()
    OS_MESSAGE_INIT_ALL();
}

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
bool os_message_send(const os_message_id_t id, const void *const data, const uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX,
                   OS_MODULE_ID_MESSAGE,
                   OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL,
                   OS_MODULE_ID_MESSAGE,
                   OS_ERROR_NULLPTR);

    bool msg_sent = false;
    os_message_t *const msg = &os_messages[id];

    os_enter_critical_section();

    /* Check if message can be pushed instantly */
    if (true == os_queue_push(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
        msg_sent = true;

        /* If there are tasks blocked on this message, release the highest priority one */
        if (msg->receive_waiting_tasks)
        {
            os_message_release_waiting_task(&msg->receive_waiting_tasks);
        }
    }
    else if (0U != timeout)
    {
        msg_sent = os_message_handle_timeout(msg, data, timeout, true);
    }
    else
    {
        /* Timetout is set to 0, just return false */
    }

    os_leave_critical_section();

    return msg_sent;
}

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
bool os_message_receive(const os_message_id_t id, void *const data, const uint32_t timeout)
{
    BEERTOS_ASSERT(id < OS_MESSAGE_ID_MAX,
                   OS_MODULE_ID_MESSAGE,
                   OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL,
                   OS_MODULE_ID_MESSAGE,
                   OS_ERROR_NULLPTR);

    bool msg_received = false;

    os_message_t *const msg = &os_messages[id];

    os_enter_critical_section();

    /* Check if message can be popped instantly */
    if (true == os_queue_pop(id + BEERTOS_QUEUE_ID_MAX, data, msg->item_size))
    {
        msg_received = true;

        /* If there are tasks blocked on this message, release the highest priority one */
        if (msg->send_waiting_tasks)
        {
            os_message_release_waiting_task(&msg->send_waiting_tasks);
        }
    }
    else if (0U != timeout)
    {
        msg_received = os_message_handle_timeout(msg, data, timeout, false);
    }
    else
    {
        /* Timetout is set to 0, just return false */
    }

    os_leave_critical_section();

    return msg_received;
}
