#include "BeeRTOS_semaphore.h"

void os_semaphore_init(os_semaphore_t *semaphore, uint32_t count)
{
    semaphore->count = count;
    semaphore->tasks_blocked = 0U;
}

bool os_semaphore_wait(os_semaphore_t *semaphore, uint32_t timeout)
{
    bool ret = false;

    os_disable_all_interrupts();

    if (semaphore->count > 0U)
    {
        semaphore->count++;
        /* ret already set to false */
    }
    else
    {
        const os_task const *current_task = os_get_current_task();
        semaphore->tasks_blocked |= (1U << current_task->priority);
        semaphore->count++;
        os_delay(timeout);
        ret = true;
    }

    os_enable_all_interrupts();

    return ret;
}

uint32_t os_get_task_id_from_mask(uint32_t mask)
{
    uint32_t idx = 0;

    while (mask)
    {
        if (mask & 1U)
        {
            return idx;
        }
        mask >>= 1U;
        idx++;
    }

    return 0U;
}

bool os_semaphore_signal(os_semaphore_t *semaphore)
{
    bool ret = false;

    os_disable_all_interrupts();

    if (semaphore->count > 0U)
    {
        semaphore->count--;
        ret = false;
    }
    else
    {
        uint32_t idx = 0U;
        uint32_t mask = semaphore->tasks_blocked;

        while (mask)
        {
            if (mask & 1)
            {
                os_task_release(idx);
                semaphore->tasks_blocked &= ~(1U << idx);
                ret = true;
            }
            mask >>= 1U;
            idx++;
        }
    }

    os_enable_all_interrupts();

    return ret;
}