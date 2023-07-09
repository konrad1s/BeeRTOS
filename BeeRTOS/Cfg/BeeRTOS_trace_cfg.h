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

#define BEERTOS_TRACE_INIT() SEGGER_SYSVIEW_Conf()

#define BEERTOS_TRACE_TICK(ticks) (void)ticks;

#define BEERTOS_TRACE_TASK_CREATE(task, name, stack_size) \
    do                                                    \
    {                                                     \
        SEGGER_SYSVIEW_OnTaskCreate((uint32_t)task);      \
        os_trace_send_task_info(task, name, stack_size);  \
    } while (0)

#define BEERTOS_TRACE_TASK_SWITCHED(task) \
    SEGGER_SYSVIEW_OnTaskStartExec((uint32_t)task)

#define BEERTOS_TRACE_TASK_READY(task) \
    SEGGER_SYSVIEW_OnTaskStartReady((uint32_t)task)

#define BEERTOS_TRACE_TASK_DELAYED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 0U)

#define BEERTOS_TRACE_SEMAPHORE_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 1U)

#define BEERTOS_TRACE_MESSAGE_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 2U)

#define BEERTOS_TRACE_MUTEX_BLOCKED(task) \
    SEGGER_SYSVIEW_OnTaskStopReady((uint32_t)task, 3U)

#define BEERTOS_TRACE_MUTEX_PRIORITY_INHERITANCE(task, priority) \
    {                                                            \
        (void)task;                                              \
        (void)priority;                                          \
    }

#define BEERTOS_TRACE_MUTEX_PRIORITY_RESTORE(task, priority) \
    {                                                        \
        (void)task;                                          \
        (void)priority;                                      \
    }

#define BEERTOS_TRACE_EXIT_ISR_SCHEDULER() \
    SEGGER_SYSVIEW_RecordExitISRToScheduler()

#define BEERTOS_TRACE_ENTER_ISR() \
    SEGGER_SYSVIEW_RecordEnterISR()

#define BEERTOS_TRACE_EXIT_ISR() \
    SEGGER_SYSVIEW_RecordExitISR()

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/
/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/
/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/
voidos_trace_send_task_info(os_task_t *task, char *name, uint32_t stack_size);

#endif /* __BEERTOS_TRACE_CFG_H__ */