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

typedef uint8_t os_sem_type_t;

typedef struct
{
    #if (BEERTOS_SEMAPHORE_COUNTING_USED == true)
    uint32_t count;
    #else
    uint8_t count;
    #endif

    os_task_mask_t tasks_blocked; /* one bit represents one task */

    /* If counting semaphores are not used, all semaphores are binary */
    #if (BEERTOS_SEMAPHORE_COUNTING_USED == true)
    os_sem_type_t type;
    #endif

} os_sem_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_task_t *os_tasks[OS_TASK_MAX];

#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, initial_count) \
    /* count, tasks_blocked */                 \
    {0U, 0U},

static os_sem_t semaphores[BEERTOS_SEMAPHORE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

static inline void os_semaphore_init(os_sem_t *sem, uint32_t count, os_sem_type_t type)
{
    BEERTOS_ASSERT(sem != NULL, OS_MODULE_ID_SEMAPHORE, OS_ERROR_NULLPTR);
    BEERTOS_ASSERT((type == SEMAPHORE_TYPE_BINARY && count <= 1U) || (type == SEMAPHORE_TYPE_COUNTING),
                   OS_MODULE_ID_SEMAPHORE,
                   OS_ERROR_INVALID_PARAM);

    sem->count = count;
    sem->tasks_blocked = 0U;
    sem->type = type;
}

/**
 * @brief Initialize semaphores
 */
void os_semaphore_module_init(void)
{
    /*! X-Macro to call os_semaphore_init for all semaphores */
    #undef BEERTOS_SEMAPHORE
    #define BEERTOS_SEMAPHORE(name, initial_count, type) \
        os_semaphore_init(&semaphores[name], initial_count, type);

    #define BEERTOS_SEMPAPHORES_INIT BEERTOS_SEMAPHORE_LIST()

    BEERTOS_SEMPAPHORES_INIT
}

/**
 * @brief Wait for semaphore
 *
 * @param id - semaphore id
 * @param timeout - maximum time to wait for semaphore
 *
 * @return true if semaphore was acquired, false if timeout occured
 */
bool os_semaphore_wait(os_sem_id_t id, uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX, OS_MODULE_ID_SEMAPHORE, OS_ERROR_INVALID_PARAM);

    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if (sem->count > 0U)
    {
        sem->count--;
        os_enable_all_interrupts();
        return true;
    }
    else
    {
        if (0U != timeout)
        {
            sem->tasks_blocked |= (1U << os_get_current_task()->priority - 1U);
            os_enable_all_interrupts();

            os_delay(timeout);

            os_disable_all_interrupts();
            bool task_released = (sem->tasks_blocked & (1U << os_get_current_task()->priority - 1U)) == 0U;
            sem->tasks_blocked &= ~(1U << os_get_current_task()->priority - 1U);
            os_enable_all_interrupts();

            return task_released;
        }
    }

    os_enable_all_interrupts();

    return false;
}

/**
 * @brief Signal semaphore
 *
 * @param id - semaphore id
 *
 * @return true if semaphore was signaled, false otherwise
 */
bool os_semaphore_signal(os_sem_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX, OS_MODULE_ID_SEMAPHORE, OS_ERROR_INVALID_PARAM);

    bool ret = true;
    os_sem_t *sem = &semaphores[id];

    os_disable_all_interrupts();

    if (sem->tasks_blocked > 0U)
    {
        os_task_t *task = os_tasks[OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(sem->tasks_blocked)];
        sem->tasks_blocked &= ~(1U << (task->priority - 1U));
        os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
    }
    else
    {
#if (BEERTOS_SEMAPHORE_COUNTING_USED == true)
        if ((SEMAPHORE_TYPE_BINARY == sem->type) &&
            (sem->count > 0U))
        {
            ret = false;
        }
        else
        {
            sem->count++;
        }
#else
        ret = false;
#endif
    }

    os_enable_all_interrupts();

    return ret;
}