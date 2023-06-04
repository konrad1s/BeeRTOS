/******************************************************************************************
 * @brief OS semaphore source file
 * @file BeeRTOS_semaphore.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_semaphore.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_assert.h"

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
    BEERTOS_ASSERT(sem != NULL, OS_MODULE_ID_SEMAPHORE, OS_ERROR_NULLPTR);

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

bool os_semaphore_wait(os_sem_id_t id, uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX, OS_MODULE_ID_SEMAPHORE, OS_ERROR_INVALID_PARAM);

    bool ret = false;
    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if (sem->count > 0U)
    {
        sem->count--;
        ret = true;
    }
    else
    {
        if(0U != timeout)
        {
            const os_task_t *current_task = os_get_current_task();
            sem->tasks_blocked |= (1U << current_task->priority);
            sem->count++;
            os_delay(timeout);
            ret = false;
        }
    }

    os_enable_all_interrupts();

    return ret;
}

bool os_semaphore_signal(os_sem_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX, OS_MODULE_ID_SEMAPHORE, OS_ERROR_INVALID_PARAM);

    bool ret = true;
    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if(sem->tasks_blocked > 0U)
    {
        uint8_t priority = OS_LOG2(sem->tasks_blocked) & 0xFFU;
        sem->tasks_blocked &= ~(1U << priority - 1U);
        os_task_release(OS_TASK_MAX - priority + 1U);
    }
    else
    {
        sem->count++;
    }

    os_enable_all_interrupts();

    return ret;
}