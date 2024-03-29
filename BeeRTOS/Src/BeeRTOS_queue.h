/******************************************************************************************
 * @brief Header file for BeeRTOS queue management system
 * @file BeeRTOS_queue.h
 * This header file defines the interface for managing queues within BeeRTOS. Queues facilitate
 * thread-safe communication between tasks via FIFO (First In, First Out) data structures. This
 * file declares the structure for queues, enumerates queue identifiers, and provides prototypes
 * for queue management functions including initialization, reset, and data push/pop operations.
 ******************************************************************************************/

#ifndef __BEERTOS_QUEUE_H__
#define __BEERTOS_QUEUE_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"

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
void os_queue_module_init(void);
void os_queue_reset(const os_queue_id_t id);
bool os_queue_is_full(const os_queue_id_t id);
bool os_queue_is_empty(const os_queue_id_t id);
bool os_queue_push(const os_queue_id_t id, const void *const data, const uint32_t len);
bool os_queue_pop(const os_queue_id_t id, void *const data, const uint32_t len);

#endif /* __BEERTOS_QUEUE_H__ */
