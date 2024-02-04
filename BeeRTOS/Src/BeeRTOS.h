/******************************************************************************************
 * @brief Main header file for BeeRTOS
 * @file BeeRTOS.h
 * This header file provides the primary interface for interacting with the BeeRTOS operating
 * system. It includes the interfaces for task management, alarms, semaphores, mutexes,
 * messages, and queues, offering a comprehensive suite of functionalities for real-time
 * multitasking applications. The file declares the initialization function for the operating
 * system, functions for entering and leaving critical sections to ensure atomic operations,
 * and a function to retrieve the current system tick count, which is essential for timing
 * and delay operations within the OS.
 ******************************************************************************************/

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
void os_init(void);
void os_enter_critical_section(void);
void os_leave_critical_section(void);
uint32_t os_get_tick_count(void);

#endif /* __BEERTOS_H__ */