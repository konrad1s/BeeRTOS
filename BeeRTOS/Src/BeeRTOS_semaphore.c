/******************************************************************************************
 * @brief OS semaphore source file
 * @file BeeRTOS_semaphore.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_semaphore.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, initial_count) \
    /* count, tasks_blocked */                 \
    { 0U, 0U },

static os_sem_t semaphores[BEERTOS_SEMAPHORE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

static inline void os_semaphore_init(os_sem_t *sem, uint32_t count)
{
    sem->count = count;
    sem->tasks_blocked = 0U;
}

void os_semaphores_init(void)
{
    /*! X-Macro to call os_semaphore_init for all semaphores */
    #undef BEERTOS_SEMAPHORE
    #define BEERTOS_SEMAPHORE(name, initial_count) \
        os_semaphore_init(&semaphores[name], initial_count);

    #define BEERTOS_SEMPAPHORES_INIT BEERTOS_SEMAPHORE_LIST

    BEERTOS_SEMPAPHORES_INIT
}

#if (BEERTOS_USE_GET_CONTROL_BLOCK_API == true)
bool os_semaphore_get_control_block_info(os_sem_id_t id, os_sem_t **sem)
{
    bool ret = false;

    if (sem != NULL)
    {
        *sem = &semaphores[id];
        ret = true;
    }

    return ret;
}
#endif

bool os_semaphore_wait(os_sem_id_t id, uint32_t timeout)
{
    bool ret = false;
    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if (sem->count > 0U)
    {
        sem->count++;
        /* ret already set to false */
    }
    else
    {
        const os_task_t const *current_task = os_get_current_task();
        sem->tasks_blocked |= (1U << current_task->priority);
        sem->count++;
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

bool os_semaphore_signal(os_sem_id_t id)
{
    bool ret = false;
    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if (sem->count > 0U)
    {
        sem->count--;
        ret = false;
    }
    else
    {
        uint32_t idx = 0U;
        uint32_t mask = sem->tasks_blocked;

        while (mask)
        {
            if (mask & 1)
            {
                os_task_release(idx);
                sem->tasks_blocked &= ~(1U << idx);
                ret = true;
            }
            mask >>= 1U;
            idx++;
        }
    }

    os_enable_all_interrupts();

    return ret;
}