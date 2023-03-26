#ifndef __BEERTOS_SEMAPHORE_H__
#define __BEERTOS_SEMAPHORE_H__

#include "BeeRTOS.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
    uint32_t count;
    uint32_t tasks_blocked; /* one bit represents one task */
} os_semaphore_t;

void os_semaphore_init(os_semaphore_t *semaphore, uint32_t count);
bool os_semaphore_wait(os_semaphore_t *semaphore, uint32_t timeout);
bool os_semaphore_signal(os_semaphore_t *semaphore);

#endif /* __BEERTOS_SEMAPHORE_H__ */