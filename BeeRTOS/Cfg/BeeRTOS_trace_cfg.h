/******************************************************************************************
 * @brief BeeRTOS Trace Configuration File
 * @file  BeeRTOS_trace_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_TRACE_CFG_H__
#define __BEERTOS_TRACE_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "SEGGER_SYSVIEW.h"
#include "BeeRTOS.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! @brief Initialize trace module */
#define BEERTOS_TRACE_INIT() {}

/*! @brief Records a tick event */
#define BEERTOS_TRACE_TICK(ticks) (void)ticks;

/*! @brief  Records a task creation event
 *  @param task - task ID
 *  @param name - task name
 * @param stack_size - task stack size
 */
#define BEERTOS_TRACE_TASK_CREATE(task, name, stack_size) \
    do                                                    \
    {                                                     \
        SEGGER_SYSVIEW_OnTaskCreate((uint32_t)task);      \
        os_trace_send_task_info(task, name, stack_size);  \
    } while (0)

/*! @brief Records a task switch event when a task is switched in for execution */
#define BEERTOS_TRACE_TASK_SWITCHED(task) \
    SEGGER_SYSVIEW_OnTaskStartExec((uint32_t)task)

/*! @brief Records a task ready event when a task becomes ready for execution */
#define BEERTOS_TRACE_TASK_READY(task) \
    SEGGER_SYSVIEW_OnTaskStartReady((uint32_t)task)

/*! @brief Records a task delay event */
#define BEERTOS_TRACE_TASK_DELAYED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 0U)

/*! @brief Records a task blocked event when a task is blocked waiting for a semaphore */
#define BEERTOS_TRACE_SEMAPHORE_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 1U)

/*! @brief Records a task blocked event when a task is blocked waiting for a message */
#define BEERTOS_TRACE_MESSAGE_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 2U)

/*! @brief Records a task blocked event when a task is blocked waiting for a mutex */
#define BEERTOS_TRACE_MUTEX_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 3U)

/*! @brief Records a priority inheritance event when 
 * a task temporarily inherits the priority of an another task */
#define BEERTOS_TRACE_MUTEX_PRIORITY_INHERITANCE(task, priority) \
    {                                                            \
        (void)task;                                              \
        (void)priority;                                          \
    }

/*! @brief Records a priority restore event when a task restores its original priority */
#define BEERTOS_TRACE_MUTEX_PRIORITY_RESTORE(task, priority) \
    {                                                        \
        (void)task;                                          \
        (void)priority;                                      \
    }

/* @brief Records the exit from an ISR to the scheduler. */
#define BEERTOS_TRACE_EXIT_ISR_SCHEDULER() \
    SEGGER_SYSVIEW_RecordExitISRToScheduler()

/* @brief Records the entry into an ISR */
#define BEERTOS_TRACE_ENTER_ISR() \
    SEGGER_SYSVIEW_RecordEnterISR()

/* @brief Records the exit from an ISR */
#define BEERTOS_TRACE_EXIT_ISR() \
    SEGGER_SYSVIEW_RecordExitISR()

/* @brief Records the timer start event */
#define BEERTOS_TRACE_ALARM_START(alarm_id, period, periodic) {}

/* @brief Records the timer stop event */
#define BEERTOS_TRACE_ALARM_CANCEL(alarm_id) {}

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/
void os_trace_send_task_info(os_task_t *task, char *name, uint32_t stack_size);

#endif /* __BEERTOS_TRACE_CFG_H__ */