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

#define OS_MSG_QUEUE_ID_MAX (OS_MESSAGE_ID_MAX + BEERTOS_QUEUE_ID_MAX)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

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

typedef enum
{
    BEERTOS_QUEUE_LIST
    BEERTOS_QUEUE_ID_MAX
} os_queue_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_queues_init(void);
void os_queue_reset(os_queue_id_t id);

bool os_queue_is_full(os_queue_id_t id);
bool os_queue_push(os_queue_id_t id, void *data, uint32_t len);
bool os_queue_is_empty(os_queue_id_t id);
bool os_queue_pop(os_queue_id_t id, void *data, uint32_t len);

#endif /* __BEERTOS_QUEUE_H__ */