#ifndef __BEE_RTOS_TASK_CFG_H__
#define __BEE_RTOS_TASK_CFG_H__

extern void task_idle(void *arg);
extern void task_app(void *arg);

#define BEERTOS_TASK_LIST \
    /* task_name, callback, priority, stacksize, autostart, task_arg */ \
    BEERTOS_TASK( OS_TASK_IDLE, task_idle, 0, 64, 1, NULL ) \
    BEERTOS_TASK( OS_TASK_APP, task_app, 1, 128, 1, NULL )

#define BEERTOS_TASK(task_name, task_cb, task_prio, task_stack, task_autostart, task_arg) task_name,

typedef enum {
    BEERTOS_TASK_LIST
} OS_TASK_ID; 



#endif /* __BEE_RTOS_TASK_CFG_H__ */