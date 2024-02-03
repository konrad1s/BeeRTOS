/******************************************************************************************
 * @brief OS header file
 * @file BeeRTOS.h
 * ****************************************************************************************/

#ifndef __BEERTOS_H__
#define __BEERTOS_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_internal.h"
#include "BeeRTOS_task.h"
#include "BeeRTOS_alarm.h"
#include "BeeRTOS_semaphore.h"
#include "BeeRTOS_mutex.h"
#include "BeeRTOS_message.h"
#include "BeeRTOS_queue.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

/**
 * @brief This function initializes the operating system. It initializes all the OS modules
 * and starts the OS scheduler (triggers the PendSV interrupt) used to switch context between
 * tasks. It should be called once by the application, before any other OS function!
 *
 * @param None
 * @return None
 */
void os_init(void);

/**
 * @brief This function enters critical section. It disables all interrupts. As many times
 * as os_enter_critical_section() is called, os_leave_critical_section() must be called to exit the critical
 * section.
 *
 * @param None
 * @return None
 */
void os_enter_critical_section(void);

/**
 * @brief This function exits critical section. If the number of calls to os_enter_critical_section()
 * is equal to the number of calls to os_leave_critical_section(), the interrupts will be enabled.
 *
 * @param None
 * @return None
 */
void os_leave_critical_section(void);

/**
 * @brief This function returns the current tick. The tick count is used to measure time in the OS.
 *
 * @param None
 * @return The current tick count (time in ticks)
 */
uint32_t os_get_tick_count(void);

#endif /* __BEERTOS_H__ */