/******************************************************************************************
 * @brief OS alarm configuration header file
 * @file BeeRTOS_alarm_cfg.h
 * ****************************************************************************************/

#ifndef __BEERTOS_ALARM_CFG_H__
#define __BEERTOS_ALARM_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*!
 *  @brief Define your alarms here
 *  @note Structure: BEERTOS_ALARM(alarm_id, callback, autostart, default_period, periodic)
 * 
 *  @param alarm_id - alarm id (created in os_alarm_id_t enum), must be unique
 *  @param callback - callback function to be called when alarm expires
 *  @param autostart - if TRUE, alarm will be started automatically after OS initialization
 *  @param default_period - default alarm period in ticks, set only if autostart is TRUE,
 *                          otherwise use os_alarm_start with period parameter
 *  @param periodic - if TRUE, alarm will be periodic, otherwise it will be one-shot,
 *                    set only if autostart is TRUE, otherwise use os_alarm_start with periodic parameter
 */
#define BEERTOS_ALARM_LIST() \
    BEERTOS_ALARM(ALARM_ONE,      alarm1_callback, false, 0U, false) \
    BEERTOS_ALARM(ALARM_TWO,      alarm2_callback, false, 0U, false) \
    BEERTOS_ALARM(ALARM_THREE,    alarm3_callback, false, 0U, false)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

extern void alarm1_callback(void);
extern void alarm2_callback(void);
extern void alarm3_callback(void);

#endif /* __BEERTOS_ALARM_CFG_H__ */