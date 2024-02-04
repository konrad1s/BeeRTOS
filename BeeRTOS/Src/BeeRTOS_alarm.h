/******************************************************************************************
 * @brief OS alarm header file
 * @file BeeRTOS_alarm.h
 * ****************************************************************************************/

#ifndef __BEERTOS_ALARM_H__
#define __BEERTOS_ALARM_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "Beertos_internal.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef BEERTOS_ALARM
#define BEERTOS_ALARM(name, callback, autostart, period, periodic) name,
/*! Alarms IDs - generated from BEERTOS_ALARM_LIST() in BeeRTOS_alarm_cfg.h */
typedef enum
{
    BEERTOS_ALARM_LIST()
    BEERTOS_ALARM_ID_MAX
} os_alarm_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_alarm_module_init(void);
void os_alarm_start(const os_alarm_id_t alarm_id, const uint32_t period, const bool periodic);
void os_alarm_cancel(const os_alarm_id_t alarm_id);
uint32_t os_alarm_get_remaining_time(const os_alarm_id_t alarm_id);
void os_alarm_tick(void);
void os_alarm_task(void *arg);

#endif /* __BEERTOS_ALARM_H__ */