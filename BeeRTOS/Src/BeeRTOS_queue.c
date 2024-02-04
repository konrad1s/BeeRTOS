/******************************************************************************************
 * @brief Source file for BeeRTOS queue management
 * @file BeeRTOS_queue.c
 * This file implements the functionality for managing queues in BeeRTOS. Queues allow for
 * thread-safe communication between tasks through FIFO data structures. This implementation
 * supports data pushing and popping, queue resetting, and checks for full or empty states.
 ******************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_queue.h"
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

/*! Macro expansions to create static buffers for queues based on configurations. */
#undef OS_QUEUE
#define OS_QUEUE(name, size) \
    static uint8_t name##_buffer[size];

#define OS_QUEUE_CREATE_BUFFERS() BEERTOS_QUEUE_LIST()
OS_QUEUE_CREATE_BUFFERS();

/******************************************************************************************/

/*! Macro expansions to create static buffers for messages based on configurations. */
#undef OS_MESSAGE
#define OS_MESSAGE(name, size, count) \
    static uint8_t name##_buffer[size * count];

#define OS_MESSAGE_CREATE_BUFFERS() OS_MESSAGES_LIST()
OS_MESSAGE_CREATE_BUFFERS();

/******************************************************************************************/

/*! Array of queue structures for managing all queues, including message queues. */
os_queue_t os_queues[OS_MSG_QUEUE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
static inline bool os_queue_can_push(const os_queue_t *const queue, const uint32_t len)
{
    return ((!queue->full) && ((queue->size - queue->head + queue->tail) % queue->size <= len));
}

static inline bool os_queue_can_pop(const os_queue_t *const queue, const uint32_t len)
{
    return ((queue->full) || (queue->size - queue->head + queue->tail) % queue->size >= len);
}

/**
 * @brief The function resets a specific queue.
 * It clears the queue, resetting the internal pointers or variables associated with the queue.
 * After calling this function, the queue is empty and ready to be used again.
 *
 * @param id - queue id
 * @return None
 */
void os_queue_reset(const os_queue_id_t id)
{
    BEERTOS_ASSERT(id < OS_MSG_QUEUE_ID_MAX,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_INVALID_PARAM);

    os_queue_t *const queue = &os_queues[id];

    os_enter_critical_section();

    queue->head = 0U;
    queue->tail = 0U;
    queue->full = false;

    os_leave_critical_section();
}

/**
 * @brief This function initializes the operating system's queues.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 *
 * @param None
 * @return None
 */
void os_queue_module_init(void)
{
    uint32_t id = 0U;

/* X-Macro to initialize all queues with their buffers and sizes */
#undef OS_QUEUE
#define OS_QUEUE(name, _size)             \
    os_queues[id].buffer = name##_buffer; \
    os_queues[id].size = _size;           \
    os_queue_reset(id);                   \
    id++;

#define BEERTOS_QUEUES_INIT_ALL() BEERTOS_QUEUE_LIST()
    BEERTOS_QUEUES_INIT_ALL();

/* X-Macro to initialize all messages with their buffers and sizes */
#undef OS_MESSAGE
#define OS_MESSAGE(name, count, _size)    \
    os_queues[id].buffer = name##_buffer; \
    os_queues[id].size = count * _size;   \
    os_queue_reset(id);                   \
    id++;

#define BEERTOS_MESSAGE_INIT_ALL() OS_MESSAGES_LIST()
    BEERTOS_MESSAGE_INIT_ALL();
}

/**
 * @brief The function checks whether a specific queue is full.
 *
 * @param id - queue id
 * @return true if the queue is full and cannot accept any more data
 *         false otherwise
 */
bool os_queue_is_full(const os_queue_id_t id)
{
    BEERTOS_ASSERT(id < OS_MSG_QUEUE_ID_MAX,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_INVALID_PARAM);

    return os_queues[id].full;
}

/**
 * @brief The function checks whether a specific queue is empty.
 *
 * @param id - queue id
 * @return true if the queue is empty and cannot provide any more data
 *         false otherwise
 */
bool os_queue_is_empty(const os_queue_id_t id)
{
    BEERTOS_ASSERT(id < OS_MSG_QUEUE_ID_MAX,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_INVALID_PARAM);

    const os_queue_t *const queue = &os_queues[id];

    os_enter_critical_section();

    /* Queue is empty if head and tail are equal and not full */
    const bool is_empty = (!queue->full && (queue->head == queue->tail));

    os_leave_critical_section();

    return is_empty;
}

/**
 * @brief The function pushes data to a specific queue.
 * It copies the data to the queue's buffer.
 *
 * @param id - queue id
 * @param data - pointer to the data to be pushed
 * @param len - length of the data to be pushed
 * @return true if the data was successfully pushed to the queue
 *         false otherwise
 */
bool os_queue_push(const os_queue_id_t id, const void *const data, const uint32_t len)
{
    BEERTOS_ASSERT(id < OS_MSG_QUEUE_ID_MAX,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_NULLPTR);

    bool ret = false;
    os_queue_t *const queue = &os_queues[id];

    os_enter_critical_section();

    /* Check if the queue can accept the data */
    if (os_queue_can_push(queue, len))
    {
        /* Copy data to the queue and update head */
        memcpy((uint8_t *)queue->buffer + queue->head, data, len);

        queue->head = (queue->head + len) % queue->size;
        queue->full = (queue->head == queue->tail);

        ret = true;
    }

    os_leave_critical_section();

    return ret;
}

/**
 * @brief The function pops data from a specific queue.
 * It copies the data from the queue's buffer.
 *
 * @param id - queue id
 * @param data - pointer to the data to be popped
 * @param len - length of the data to be popped
 * @return true if the data was successfully popped from the queue
 *         false otherwise
 */
bool os_queue_pop(const os_queue_id_t id, void *const data, const uint32_t len)
{
    BEERTOS_ASSERT(id < OS_MSG_QUEUE_ID_MAX,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(data != NULL,
                   OS_MODULE_ID_QUEUE,
                   OS_ERROR_NULLPTR);

    uint8_t ret = false;
    os_queue_t *const queue = &os_queues[id];

    os_enter_critical_section();

    /* Check if the queue can provide the data */
    if (os_queue_can_pop(queue, len))
    {
        /* Copy data from the queue and update tail */
        memcpy(data, (uint8_t *)queue->buffer + queue->tail, len);

        queue->tail = (queue->tail + len) % queue->size;
        queue->full = false;

        ret = true;
    }

    os_leave_critical_section();

    return ret;
}
