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
#include "BeeRTOS_alarm_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef BEERTOS_ALARM
#define BEERTOS_ALARM(name, callback, autostart, period, periodic) name,
typedef enum
{
    BEERTOS_ALARM_LIST()
    BEERTOS_ALARM_ID_MAX
} os_alarm_id_t;

typedef struct
{
    uint32_t    period;
    bool        periodic;
    void        (*callback)(void);
} os_alarm_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_alarm_init(void);
void os_alarm_start(os_alarm_id_t alarm_id, uint32_t period, bool periodic);
void os_alarm_cancel(os_alarm_id_t alarm_id);
void os_alarm_tick(void);
uint32_t os_alarm_get_remaining_time(os_alarm_id_t alarm_id);

#endif /* __BEERTOS_ALARM_H__ */