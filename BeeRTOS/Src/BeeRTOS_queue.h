/******************************************************************************************
 * @brief OS queue header file
 * @file BeeRTOS_queue.h
 * ****************************************************************************************/

#ifndef __BEERTOS_QUEUE_H__
#define __BEERTOS_QUEUE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"
#include "BeeRTOS_queue_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! Maximum number of queues (normal queues + message queues) */
#define OS_MSG_QUEUE_ID_MAX (OS_MESSAGE_ID_MAX + BEERTOS_QUEUE_ID_MAX)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/*! Structure describing a queue (circular buffer) */
typedef struct
{
    void *buffer;
    uint32_t size;
    uint32_t head;
    uint32_t tail;
    bool full;
} os_queue_t;

#undef OS_QUEUE
#define OS_QUEUE(name, ...) name,

/*! Queue IDs - generated from BEERTOS_QUEUE_LIST() in BeeRTOS_queue_cfg.h */
typedef enum
{
    BEERTOS_QUEUE_LIST()
    BEERTOS_QUEUE_ID_MAX
} os_queue_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

/**
 * @brief This function initializes the operating system's queues.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 *
 * @param None
 * @return None
 */
void os_queue_init(void);

/**
 * @brief The function resets a specific queue.
 * It clears the queue, resetting the internal pointers or variables associated with the queue.
 * After calling this function, the queue is empty and ready to be used again.
 *
 * @param id - queue id
 * @return None
 */
void os_queue_reset(os_queue_id_t id);

/**
 * @brief The function checks whether a specific queue is full.
 *
 * @param id - queue id
 * @return true if the queue is full and cannot accept any more data
 *         false otherwise
 */
bool os_queue_is_full(os_queue_id_t id);

/**
 * @brief The function checks whether a specific queue is empty.
 *
 * @param id - queue id
 * @return true if the queue is empty and cannot provide any more data
 *         false otherwise
 */
bool os_queue_is_empty(os_queue_id_t id);

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
bool os_queue_push(os_queue_id_t id, void *data, uint32_t len);

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
bool os_queue_pop(os_queue_id_t id, void *data, uint32_t len);

#endif /* __BEERTOS_QUEUE_H__ */
