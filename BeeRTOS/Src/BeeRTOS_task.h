/******************************************************************************************
 * @brief Header file for BeeRTOS task management
 * @file BeeRTOS_task.h
 * This header file defines the interface for managing tasks within BeeRTOS. It declares
 * the structure for task control blocks, task identifiers, and provides prototypes for
 * functions to initialize the task system, create tasks, start and stop tasks, delay tasks,
 * and process task scheduling. The file supports task stack monitoring configurations to
 * ensure stack integrity and prevent stack overflows. It also defines utility macros for
 * task management and scheduling.
 * ****************************************************************************************/

#ifndef __BEERTOS_TASK_H__
#define __BEERTOS_TASK_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"
#include "BeeRTOS_mutex.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

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
#undef BEERTOS_MUTEX
#undef BEERTOS_ALARM_TASK

#define BEERTOS_TASK(task_name, ...) task_name,
#define BEERTOS_MUTEX(task_name, ...) PRIO_CELLING_TASK_##task_name,
#define BEERTOS_ALARM_TASK(task_name, ...) task_name,

/*! Task IDs - generated from BEERTOS_PRIORITY_LIST() in BeeRTOS_task_cfg.h */
typedef enum
{
    OS_TASK_IDLE = 0, /* Reserved for idle task! */
    BEERTOS_PRIORITY_LIST()
    OS_TASK_MAX
} os_task_id_t;

/******************************************************************************************/

#undef BEERTOS_TASK
#undef BEERTOS_MUTEX
#undef BEERTOS_ALARM_TASK

#define BEERTOS_TASK(...) +1U
#define BEERTOS_MUTEX(...) +1U
#define BEERTOS_ALARM_TASK(...) +1U

/*! Returns the number of tasks, OS_TASK_MAX cannot be used in preprocessor expressions,
    because enum is known only after the preprocessor is done */
#define OS_TASK_COUNT (1U + BEERTOS_PRIORITY_LIST())

/******************************************************************************************/

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
void os_task_module_init(void);
bool os_task_start(const os_task_id_t task_id);
bool os_task_stop(const os_task_id_t task_id);
void os_task_release(const os_task_id_t task_id);
void os_task_delete(void);
void os_delay(const uint32_t ticks);
void os_task_tick(void);
void os_sched(void);

#endif /* __BEERTOS_TASK_H__ */