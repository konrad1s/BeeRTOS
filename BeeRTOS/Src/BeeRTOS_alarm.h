/******************************************************************************************
 * @brief Header file for BeeRTOS alarm functionality
 * @file BeeRTOS_alarm.h
 * This header file defines the interface for the BeeRTOS alarm system, which allows tasks to
 * schedule actions for specific times or after specific intervals. It includes definitions
 * for alarm IDs, function prototypes for alarm management, and the initialization function.
 ******************************************************************************************/

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
/*! Enumerates alarm IDs generated from the BEERTOS_ALARM_LIST macro. 
 *  This enumeration provides a unique identifier for each alarm defined using the BEERTOS_ALARM
 *  macro within the BEERTOS_ALARM_LIST macro expansion. */
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