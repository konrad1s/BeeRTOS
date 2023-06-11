#include "BeeRTOS.h"
#include "BeeRTOS_queue.h"
#include <string.h>

typedef struct
{
    void *buffer;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    bool full;
} os_queue_t;

#undef OS_QUEUE
#define OS_QUEUE(name, size) \
    static uint8_t name##_buffer[size];

#define BEERTOS_QUEUES_LIST OS_QUEUES_LIST

BEERTOS_QUEUES_LIST

static os_queue_t queues[OS_QUEUE_ID_MAX];

void os_queue_reset(os_queue_id_t id)
{
    os_queue_t *queue = &queues[id];

    queue->head = 0U;
    queue->tail = 0U;
    queue->full = false;
}

void os_queues_init(void)
{
    uint32_t id = 0U;
#undef OS_QUEUE
#define OS_QUEUE(name, _size)          \
    queues[id].buffer = name##_buffer; \
    queues[id].size = _size;           \
    os_queue_reset(id);                \
    id++;

#define BEERTOS_QUEUES_INIT_ALL() OS_QUEUES_LIST

    BEERTOS_QUEUES_INIT_ALL();
}

bool os_queue_is_full(os_queue_id_t id)
{
    // BEERTOS_ASSERT(queue != NULL, OS_MODULE_ID_QUEUE, OS_ERROR_NULLPTR);
    return queues[id].full;
}

static bool os_queue_check_push(os_queue_t *queue, uint32_t len)
{
    return ((queue->full) || (((queue->size - queue->head + queue->tail) % queue->size >= len)));
}

static bool os_queue_check_pop(os_queue_t *queue, uint32_t len)
{
    return ((!queue->full) && ((queue->size - queue->tail + queue->head % queue->size) >= len));
}

bool os_queue_push(os_queue_id_t id, void *data, uint32_t len)
{
    // BEERTOS_ASSERT(queue != NULL, OS_MODULE_ID_QUEUE, OS_ERROR_NULLPTR);
    // BEERTOS_ASSERT(data != NULL, OS_MODULE_ID_QUEUE, OS_ERROR_NULLPTR);

    bool ret = false;
    os_queue_t *queue = &queues[id];

    if (!os_queue_check_push(queue, len))
    {
        memcpy((uint8_t *)queue->buffer + queue->head, data, len);

        queue->head = (queue->head + len) % queue->size;
        queue->full = (queue->head == queue->tail);

        ret = true;
    }
}

bool os_queue_is_empty(os_queue_id_t id)
{
    // BEERTOS_ASSERT(queue != NULL, OS_MODULE_ID_QUEUE, OS_ERROR_NULLPTR);
    const os_queue_t *queue = &queues[id];
    return (!queue->full && (queue->head == queue->tail));
}

bool os_queue_pop(os_queue_id_t id, void *data, uint32_t len)
{
    // BEERTOS_ASSERT(queue != NULL, OS_MODULE_ID_QUEUE, OS_ERROR_NULLPTR);

    uint8_t ret = false;
    os_queue_t *queue = &queues[id];

    if (!os_queue_check_pop(queue, len))
    {
        memcpy(data, (uint8_t *)queue->buffer + queue->tail, len);

        queue->tail = (queue->tail + len) % queue->size;
        queue->full = false;

        ret = true;
    }

    return ret;
}