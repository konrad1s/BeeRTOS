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

/**
 * @brief The function initializes the operating system's alarms.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 * It is also responsible for starting the alarms that are set to autostart.
 *
 * @param None
 * @return None
 */
void os_alarm_module_init(void);

/**
 * @brief This function starts the specified alarm. If the alarm is already started, the function
 * will restart it. If the alarm is already started and periodic, the function will restart it
 * with the new period.
 * 
 * @param alarm_id - alarm id
 * @param period - alarm period in ticks
 * @param periodic - true if the alarm is periodic, false if it is one-shot
 * @return None
 */
void os_alarm_start(const os_alarm_id_t alarm_id, const uint32_t period, const bool periodic);

/**
 * @brief This function cancels the specified alarm. The alarm has to be started before it can be
 * cancelled.
 * 
 * @param alarm_id - alarm id
 * @return None
 */
void os_alarm_cancel(const os_alarm_id_t alarm_id);

/**
 * @brief This function returns the remaining time until the alarm expires.
 * If the alarm is not started, the function will return 0.
 * 
 * @param alarm_id - alarm id
 * @return remaining time in ticks
 */
uint32_t os_alarm_get_remaining_time(const os_alarm_id_t alarm_id);

/**
 * @brief This function is called by the OS tick handler. It is responsible for decrementing the
 * remaining time of the active alarms and calling the alarm callbacks when the remaining time
 * reaches 0.
 * 
 * @param None
 * @return None
 */
void os_alarm_tick(void);

#endif /* __BEERTOS_ALARM_H__ */