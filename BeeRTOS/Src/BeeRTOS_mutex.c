/******************************************************************************************
 * @brief OS mutex source file
 * @file BeeRTOS_mutex.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_mutex.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_assert.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/* Structure to hold mutex data */
typedef struct
{
    uint32_t count;               /* number of times the mutex is locked */
    os_task_mask_t tasks_blocked; /* tasks blocked on the mutex */
    os_task_t *owner_task;        /* task that owns the mutex */
    uint8_t owner_priority;       /* priority of the task that owns the mutex */
} os_mutex_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_task_t *os_tasks[OS_TASK_MAX];

/*! List of all mutexes */
static os_mutex_t os_mutexes[BEERTOS_MUTEX_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

void os_mutex_init(void)
{
    /*! X-Macro to initialize all mutexes */
    #undef BEERTOS_MUTEX
    #define BEERTOS_MUTEX(name, initial_count)  \
        os_mutexes[name].count = initial_count; \
        os_mutexes[name].tasks_blocked = 0U;    \
        os_mutexes[name].owner_task = NULL;     \
        os_mutexes[name].owner_priority = 0U;

    #define BEERTOS_MUTEXES_INIT_ALL() BEERTOS_MUTEX_LIST()

    BEERTOS_MUTEXES_INIT_ALL();
}

void os_mutex_lock(os_mutex_id_t id, uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX, OS_MODULE_ID_MUTEX, OS_ERROR_INVALID_PARAM);

    os_mutex_t *const mutex = &os_mutexes[id];
    const os_task_t *const task = os_get_current_task();

    os_disable_all_interrupts();

    /* If the mutex is not locked, lock it */
    if (NULL == mutex->owner_task)
    {
        mutex->owner_task = task;
        mutex->count++;
    }
    else
    {
        if (0U != timeout)
        {
            /* Priority inheritance.
             * If the owner of the mutex has lower priority than the task that wants to lock it,
             * the owner's priority is set to the task's priority */
            if (mutex->owner_task->priority < task->priority)
            {
                mutex->owner_priority = mutex->owner_task->priority;
                mutex->owner_task->priority = task->priority;
            }
            mutex->tasks_blocked |= (1U << task->priority - 1U);
            os_delay(timeout);
        }
    }
    os_enable_all_interrupts();
}

void os_mutex_unlock(os_mutex_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX, OS_MODULE_ID_MUTEX, OS_ERROR_INVALID_PARAM);

    os_mutex_t *mutex = &os_mutexes[id];

    os_disable_all_interrupts();

    /* Check if the mutex is locked by the calling task */
    if (mutex->owner_task == os_get_current_task())
    {
        mutex->count--;
        if (mutex->count == 0U)
        {
            /* If there are tasks blocked on the mutex, unblock the one with the highest priority */
            const os_task_t *const high_prio_task = os_tasks[OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mutex->tasks_blocked)];
            mutex->owner_task->priority = mutex->owner_priority;
            mutex->owner_task = high_prio_task;
            mutex->count++;
            mutex->tasks_blocked &= ~(1U << (high_prio_task->priority - 1U));
            os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(high_prio_task->priority));
        }
    }
    else
    {
        /* TODO assert */
    }

    os_enable_all_interrupts();
}