/******************************************************************************************
 * @brief OS task header file
 * @file BeeRTOS_task.h
 * ****************************************************************************************/

#ifndef __BEERTOS_TASK_H__
#define __BEERTOS_TASK_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"
#include "BeeRTOS_task_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! Maximum number of tasks, that can be created */
#define OS_MAX_TASK_NB (32U)

/*! Returns the task ID from the priority */
#define OS_GET_TASK_ID_FROM_PRIORITY(priority) (OS_TASK_MAX - priority)

/*! Task stack pattern used for stack monitoring */
#define OS_TASK_STACK_PATTERN ((os_stack_t)0xA5A5A5A5U)

#if (BEERTOS_USE_TASK_STACK_MONITOR == true) && \
    (BEERTOS_USE_FAST_STACK_MONITOR == true) && \
    (BEERTOS_USE_USER_STACK_MONITOR == true)
#error "Only one stack monitor can be used at a time!"
#endif

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/*! OS Thread control block */
typedef struct
{
    volatile void *sp; /*!< stack pointer */
    uint32_t ticks;    /*!< ticks */
    uint8_t priority;  /*!< priority */
} os_task_t;

typedef void (*os_task_handler)(void *args);

#undef BEERTOS_TASK
#define BEERTOS_TASK(task_name, ...) task_name,
/*! Task IDs - generated from BEERTOS_TASK_LIST() in BeeRTOS_task_cfg.h */
typedef enum
{
    OS_TASK_IDLE = 0, /* Reserved for idle task! */
    BEERTOS_TASK_LIST()
    OS_TASK_MAX
} os_task_id_t;

#undef BEERTOS_TASK
#define BEERTOS_TASK(...) +1U
/*! Returns the number of tasks, OS_TASK_MAX cannot be used in preprocessor expressions,
    because enum is known only after the preprocessor is done */
#define OS_TASK_COUNT (1U + BEERTOS_TASK_LIST())

/*! Check and select the proper mask type for the number of configured tasks */
#if OS_TASK_COUNT <= 8U
    typedef uint8_t os_task_mask_t;
    #define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask) OS_GET_HIGHEST_PRIO_TASK_FROM_MASK8(mask)
#elif OS_TASK_COUNT <= 16U
    typedef uint16_t os_task_mask_t;
    #define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask) OS_GET_HIGHEST_PRIO_TASK_FROM_MASK16(mask)
#elif OS_TASK_COUNT <= 32U
    typedef uint32_t os_task_mask_t;
    #define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask) OS_GET_HIGHEST_PRIO_TASK_FROM_MASK32(mask)
#elif OS_TASK_COUNT <= 64U
    typedef uint64_t os_task_mask_t;
    #define OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask) OS_GET_HIGHEST_PRIO_TASK_FROM_MASK64(mask)
#else
    #error "OS_TASK_MAX must be less or equal to 64"
#endif

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

/**
 * @brief This function initializes the operating system's tasks.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 * If the task auto start is enabled, the task will be started.
 *
 * @param id - None
 * @return None
 */
void os_task_init(void);

/**
 * @brief This function starts the specified task.
 *
 * @param task_id - id of the task to be started
 * @return None
 */
bool os_task_start(os_task_id_t task_id);

/**
 * @brief This function stops the specified task.
 *
 * @param task_id - id of the task to be stopped
 * @return None
 */
bool os_task_stop(os_task_id_t task_id);

/**
 * @brief Release the task that was previously suspended (for example by a mutex or a semaphore)
 * After calling this function, the scheduler is called to switch context to another task.
 *
 * @param task_id - id of the task to be released
 */
void os_task_release(os_task_id_t task_id);

/**
 * @brief This function deletes (removes from scheduler) the current task.
 * Becasue current implementation does not support dynamic memory allocation,
 * the task stack is not freed. This function only removes the task from the
 * list of tasks to be scheduled.
 * After calling this function, the scheduler is called to switch context to another task.
 *
 * @param None
 * @return None
 */
void os_task_delete(void);

/**
 * @brief This function returns the current task (the task that is currently calling this function).
 * Used for mutexes/semaphores/queues and other functions that manage resources.
 *
 * @return os_task_t* - pointer to the current task
 */
os_task_t *os_get_current_task(void);

/**
 * @brief Delay the current task for the specified number of ticks.
 * After the specified number of ticks, the task will be ready to run again.
 * After calling this function, the scheduler is called to switch context to another task.
 *
 * @param ticks - number of ticks to delay, must be > 0
 * @return None
 */
void os_delay(uint32_t ticks);

/**
 * @brief Process ticks for all tasks. This function is called by the system tick handler.
 *
 * @param None
 * @return None
 */
void os_task_tick(void);

/**
 * @brief Function used for scheduling tasks. This function is called by the system tick handler.
 *
 * @param None
 * @return None
 */
void os_sched(void);

#endif /* __BEERTOS_TASK_H__ */