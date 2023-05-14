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

#define BEERTOS_TRACE_TASK_CREATE(task, name, stack_size)       \
            SEGGER_SYSVIEW_OnTaskCreate((uint32_t)task);        \
            os_trace_send_task_info(task, name, stack_size);

#define BEERTOS_TRACE_TASK_SWITCHED(task)   SEGGER_SYSVIEW_OnTaskStartExec((uint32_t)task)

#define BEERTOS_TRACE_EXIT_ISR_SCHEDULER()  SEGGER_SYSVIEW_RecordExitISRToScheduler()

#define BEERTOS_TRACE_ENTER_ISR()           SEGGER_SYSVIEW_RecordEnterISR()
#define BEERTOS_TRACE_EXIT_ISR()            SEGGER_SYSVIEW_RecordExitISR()

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