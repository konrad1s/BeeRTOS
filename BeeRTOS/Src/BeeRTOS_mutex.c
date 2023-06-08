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

typedef struct
{
    uint32_t count;
    uint32_t tasks_blocked;
    os_task_t *owner;
    uint8_t owner_priority;
} os_mutex_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

extern os_task_t *os_tasks[OS_TASK_MAX];
static os_mutex_t mutexes[BEERTOS_MUTEX_ID_MAX];

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

void os_mutex_lock(os_mutex_id_t id, uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX, OS_MODULE_ID_MUTEX, OS_ERROR_INVALID_PARAM);

    os_mutex_t *mutex = &mutexes[id];
    os_task_t *task = os_get_current_task();

    os_disable_all_interrupts();

    if (NULL == mutex->owner)
    {
        mutex->owner = task;
        mutex->count++;
    }
    else
    {
        if (0U != timeout)
        {
            if (mutex->owner->priority < task->priority)
            {
                mutex->owner_priority = mutex->owner->priority;
                mutex->owner->priority = task->priority;
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

    os_mutex_t *mutex = &mutexes[id];
    os_task_t *task = os_get_current_task();

    os_disable_all_interrupts();

    if (mutex->owner == task)
    {
        mutex->count--;
        if (mutex->count == 0U)
        {
            mutex->owner->priority = mutex->owner_priority;
            os_task_t* task = os_tasks[OS_LOG2(mutex->tasks_blocked) & 0xFFU];
            mutex->owner = task;
            mutex->count++;
            mutex->tasks_blocked &= ~(1U << (task->priority - 1U));
            os_task_release(OS_GET_TASK_ID_FROM_PRIORITY(task->priority));
        }
    }
    else
    {
        /* TODO assert */
    }

    os_enable_all_interrupts();
}