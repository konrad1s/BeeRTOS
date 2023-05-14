#include "BeeRTOS_trace_cfg.h"

void os_trace_send_task_info(os_task_t *task, char *name, uint32_t stack_size)
{
    SEGGER_SYSVIEW_TASKINFO info;

    info.TaskID = (uint32_t)task;
    info.sName = name;
    info.Prio = task->priority;
    info.StackBase = (uint32_t)task->sp;
    info.StackSize = stack_size;

    SEGGER_SYSVIEW_SendTaskInfo(&info);
}