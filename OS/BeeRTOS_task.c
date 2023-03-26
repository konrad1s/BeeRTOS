/* */
#include "BeeRTOS.h"
#include "BeeRTOS_task_cfg.h"


#undef BEERTOS_TASK
#define BEERTOS_TASK(task_name, task_cb, task_prio, task_stack, task_autostart, task_arg) \
    static uint32_t task_name##_stack[task_stack];

BEERTOS_TASK_LIST


void os_create_all_tasks(void)
{
    return 0;
}