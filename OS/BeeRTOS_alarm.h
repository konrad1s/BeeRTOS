#ifndef __BEERTOS_ALARM_H__
#define __BEERTOS_ALARM_H__

#include "BeeRTOS_alarm_cfg.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
    uint32_t    period;
    bool        periodic;
    void        (*callback)(void);
} alarm_t;

void os_alarms_init(void);
void os_alarm_start(alarm_id_t alarm_id, uint32_t period, bool periodic);
void os_alarm_cancel(alarm_id_t alarm_id);
void os_alarm_tick(void);

#endif /* __BEERTOS_ALARM_H__ */