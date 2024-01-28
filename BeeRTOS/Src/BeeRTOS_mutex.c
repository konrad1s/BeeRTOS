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
    uint32_t count;                 /* number of times the mutex is locked */
    os_task_mask_t tasks_blocked;   /* tasks blocked on the mutex */
    os_task_t *owner_task;          /* task that currently owns the mutex */
    os_task_t *original_owner_task; /* task that originally owned the mutex */
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

void os_mutex_module_init(void)
{
/*! X-Macro to initialize all mutexes */
#undef BEERTOS_MUTEX
#define BEERTOS_MUTEX(name, initial_count)  \
    os_mutexes[name].count = initial_count; \
    os_mutexes[name].tasks_blocked = 0U;    \
    os_mutexes[name].owner_task = NULL;     \
    os_mutexes[name].original_owner_task = NULL;
#undef BEERTOS_TASK
#define BEERTOS_TASK(...)

#define BEERTOS_MUTEXES_INIT_ALL() BEERTOS_MUTEX_LIST()

    BEERTOS_MUTEXES_INIT_ALL();
}

void os_mutex_lock(os_mutex_id_t id, uint32_t timeout)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX, OS_MODULE_ID_MUTEX, OS_ERROR_INVALID_PARAM);

    os_mutex_t *const mutex = &os_mutexes[id];
    const os_task_t *const task = os_get_current_task();

    os_disable_all_interrupts();

    os_enable_all_interrupts();
}

void os_mutex_unlock(os_mutex_id_t id)
{
    BEERTOS_ASSERT(id < BEERTOS_MUTEX_ID_MAX, OS_MODULE_ID_MUTEX, OS_ERROR_INVALID_PARAM);

    os_mutex_t *mutex = &os_mutexes[id];

    os_disable_all_interrupts();

    os_enable_all_interrupts();
}