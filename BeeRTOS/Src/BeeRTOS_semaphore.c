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
#include "BeeRTOS_trace_cfg.h"

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
extern os_task_t *volatile os_task_current;

#undef BEERTOS_SEMAPHORE
#define BEERTOS_SEMAPHORE(name, initial_count) \
    /* count, tasks_blocked */                 \
    {0U, 0U},

static os_sem_t semaphores[BEERTOS_SEMAPHORE_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

static inline void os_semaphore_init(os_sem_t *const sem,
                                     const uint32_t count,
                                     const os_sem_type_t type)
{
    BEERTOS_ASSERT(sem != NULL, OS_MODULE_ID_SEMAPHORE, OS_ERROR_NULLPTR);
    BEERTOS_ASSERT((type == SEMAPHORE_TYPE_BINARY && count <= 1U) || (type == SEMAPHORE_TYPE_COUNTING),
                   OS_MODULE_ID_SEMAPHORE,
                   OS_ERROR_INVALID_PARAM);

    sem->count = count;
    sem->tasks_blocked = 0U;
    sem->type = type;
}

static void os_sem_unlock_waiting_task(os_sem_t *const sem)
{
    /* There are tasks blocked on the semaphore, release the highest priority one */
    const os_task_id_t highest_prio_task = OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(sem->tasks_blocked);
    os_task_t *const task = os_tasks[highest_prio_task];

    sem->tasks_blocked &= ~(1U << (task->priority - 1U));
    os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
    BEERTOS_TRACE_SEMAPHORE_UNBLOCKED(task);
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

#define BEERTOS_SEMPAPHORES_INIT() BEERTOS_SEMAPHORE_LIST()

    BEERTOS_SEMPAPHORES_INIT();
}

/**
 * @brief Wait for semaphore
 *
 * @param id - semaphore id
 * @param timeout - maximum time to wait for semaphore
 *
 * @return true if semaphore was acquired, false if timeout occured
 */
bool os_semaphore_wait(const os_sem_id_t id, const uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX,
                   OS_MODULE_ID_SEMAPHORE,
                   OS_ERROR_INVALID_PARAM);

    bool s_got = true;
    os_sem_t *const sem = &semaphores[id];
    const uint8_t current_task_priority = os_task_current->priority;

    os_enter_critical_section();

    if (sem->count > 0U)
    {
        /* Decrement the semaphore count, s_got is true */
        sem->count--;
    }
    else if (0U != timeout)
    {
        /* Block the task and wait for the semaphore */
        sem->tasks_blocked |= (1U << (current_task_priority - 1U));
        os_delay(timeout);
        BEERTOS_TRACE_SEMAPHORE_BLOCKED(os_task_current);

        os_leave_critical_section();
        /* Potencial context switch is right here */
        os_enter_critical_section();

        /* Check if the task was unblocked by the semaphore */
        s_got = (sem->tasks_blocked & (1U << (current_task_priority - 1U))) == 0U;
        /* Clear the waiting bit */
        sem->tasks_blocked &= ~(1U << (current_task_priority - 1U));
    }
    else
    {
        /* There is no timeout and the semaphore is not available */
        s_got = false;
    }

    os_leave_critical_section();

    return s_got;
}

/**
 * @brief Signal semaphore
 *
 * @param id - semaphore id
 *
 * @return true if semaphore was signaled, false otherwise
 */
bool os_semaphore_signal(const os_sem_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_SEMAPHORE_ID_MAX,
                   OS_MODULE_ID_SEMAPHORE,
                   OS_ERROR_INVALID_PARAM);

    bool s_signaled = true;
    os_sem_t *const sem = &semaphores[id];

    os_enter_critical_section();

    if (sem->tasks_blocked > 0U)
    {
        os_sem_unlock_waiting_task(sem);
    }
    else
    {
        /* If semaphore is counting, or binary and not signaled
           increment the count */
        if (!sem->count ||
#if (BEERTOS_SEMAPHORE_COUNTING_USED == true)
            sem->type == SEMAPHORE_TYPE_COUNTING
#endif
        )
        {
            sem->count++;
        }
        else
        {
            /* Binary semaphore is already signaled */
            s_signaled = false;
        }
    }

    os_leave_critical_section();

    return s_signaled;
}