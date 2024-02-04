/******************************************************************************************
 * @brief Source file for BeeRTOS mutex management
 * @file BeeRTOS_mutex.c
 * This file implements mutex functionality for BeeRTOS, providing mechanisms for mutual
 * exclusion and synchronization among tasks. It includes operations for initializing
 * mutexes, locking and unlocking mutexes with support for recursive locking,
 * and priority inheritance through the priority ceiling protocol to prevent
 * priority inversion issues.
 ******************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_mutex.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_assert.h"
#include "BeeRTOS_trace_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/* Structure to hold mutex data */
typedef struct
{
    os_task_t *owner;       /* task that owns the mutex */
    os_task_t **pcp_task;   /* priority ceiling protocol task */
    uint8_t locks_nb;       /* number of locks */
    uint8_t owner_priority; /* original priority of the owner task */
    uint8_t pcp_priority;   /* priority ceiling protocol priority */
} os_mutex_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_task_t *os_tasks[OS_TASK_MAX];
extern os_task_t *volatile os_task_current;

/*! List of all mutexes */
static os_mutex_t os_mutexes[BEERTOS_MUTEX_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
/**
 * @brief This function initializes all mutexes.
 * 
 * @return None
 */
void os_mutex_module_init(void)
{
    uint32_t idx = OS_TASK_MAX - 1U;

    #undef BEERTOS_MUTEX
    #undef BEERTOS_TASK
    #undef BEERTOS_ALARM_TASK

    /* Here is the X-Macro to initialize all mutexes, from user configuration */
    #define BEERTOS_MUTEX(name, initial_count)      \
        os_mutexes[name].locks_nb = initial_count;  \
        os_mutexes[name].owner = NULL;              \
        os_mutexes[name].pcp_task = &os_tasks[idx]; \
        os_mutexes[name].owner_priority = 0U;       \
        os_mutexes[name].pcp_priority = idx;        \
        idx--;

    #define BEERTOS_TASK(...) \
        idx--;

    #define BEERTOS_ALARM_TASK(...) \
        idx--;

    #define OS_MUTEXES_INIT_ALL() BEERTOS_MUTEX_LIST()
    OS_MUTEXES_INIT_ALL();
}

/**
 * @brief This function locks the specified mutex. If the mutex is already locked, the function
 * will wait until the mutex is unlocked (with timeout). If the mutex is already locked by the
 * calling task, recursive locking is performed. The mutex must be unlocked as many times as it
 * was locked.
 * 
 * @param id - mutex id
 * @param timeout - maximum time to wait for the mutex to be unlocked. If timeout is 0, the function
 *                  will return immediately false if the mutex is locked.
 */
bool os_mutex_lock(const os_mutex_id_t id, const uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_PARAM);

    os_mutex_t *const mutex = &os_mutexes[id];
    os_task_t *const current_task = os_task_current;

    /* The priority ceiling task must have a priority of all
       tasks that can lock the mutex, otherwise priority inversion,
       deadlock or other issues can occur */
    BEERTOS_ASSERT(current_task->priority <= mutex->pcp_priority,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_PARAM);

    /* In the current implementation, there is no option do not use
       priority ceiling protocol, so when this function is called the
       mutex->owner must be equal to NULL because the PCP has always
       higher priority than the task that is trying to lock the mutex */
    BEERTOS_ASSERT(mutex->owner == NULL,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_PARAM);

    /* The number of locks must be less than 255, otherwise overflow */
    BEERTOS_ASSERT(mutex->locks_nb < 255U,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_OVERFLOW);

    os_enter_critical_section();

    /* Check if the mutex is available */
    if (mutex->owner == NULL)
    {
        /* Mutex is available, lock it */
        mutex->owner = current_task;
        /* This is the first lock, so set the locks_nb to 1 */
        mutex->locks_nb = 1U;

        /* Apply priority ceiling protocol */
        mutex->owner_priority = current_task->priority;
        current_task->priority = mutex->pcp_priority;
        *mutex->pcp_task = current_task;

        /* Start the priority ceiling task */
        os_task_start(OS_GET_TASK_ID_FROM_PRIORITY((*mutex->pcp_task)->priority));
        BEERTOS_TRACE_MUTEX_PRIORITY_INHERITANCE(current_task,
                                                 current_task->priority);
        os_sched();
    }
    else if (mutex->owner == current_task)
    {
        /* The task already owns the mutex, so increment the nested locks counter */
        mutex->locks_nb++;
    }
    else if (0U != timeout)
    {
        /* Currently this is configuration error, please check the
           BEERTOS_ASSERT(mutex->owner != current_task... description,
           left here for future implementation */

        if (mutex->owner == current_task)
        {
            /* The task was unblocked by calling os_mutex_unlock(),
               so it is the owner of the mutex now */
        }
        else
        {
            /* The task was unblocked by the timeout, so it did not acquire the mutex */
        }
    }
    else
    {
        /* Mutex is not available */
    }

    os_leave_critical_section();

    return (mutex->owner == current_task);
}

/**
 * @brief This function unlocks the specified mutex. Can be called only if the mutex is locked.
 * If the mutex is locked by another task, the function will rise an error. If the mutex was
 * locked multiple times, it must be unlocked the same number of times.
 *
 * @param id - mutex id
 * @return None
 */
void os_mutex_unlock(const os_mutex_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_PARAM);

    os_mutex_t *const mutex = &os_mutexes[id];
    os_task_t *const current_task = os_task_current;

    /* Only the owner of the mutex can unlock it */
    BEERTOS_ASSERT(mutex->owner == current_task,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_PARAM);
    /* The number of locks must be greater than 0 */
    BEERTOS_ASSERT(mutex->locks_nb > 0U,
                   OS_MODULE_ID_MUTEX,
                   OS_ERROR_INVALID_OPERATION);

    os_enter_critical_section();

    if (mutex->locks_nb > 0U)
    {
        /* Decrement the nested locks counter */
        mutex->locks_nb--;
        if (mutex->locks_nb == 0U)
        {
            /* Restore the original priority of the task that owns the mutex */
            current_task->priority = mutex->owner_priority;
            mutex->owner = NULL;
            os_task_stop(OS_GET_TASK_ID_FROM_PRIORITY(mutex->pcp_priority));
            BEERTOS_TRACE_MUTEX_PRIORITY_RESTORE(current_task,
                                                 current_task->priority);
            os_sched();

            /* We should check here if there are tasks blocked on the mutex,
               but in the current implementaion only the mutex PCP is supported,
               so there is no need to check it */
        }
    }

    os_leave_critical_section();
}