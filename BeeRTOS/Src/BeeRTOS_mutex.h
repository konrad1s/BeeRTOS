/******************************************************************************************
* @brief OS mutex header file
* @file BeeRTOS_mutex.h
* ****************************************************************************************/

#ifndef __BEERTOS_MUTEX_H__
#define __BEERTOS_MUTEX_H__

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

#undef BEERTOS_MUTEX
#define BEERTOS_MUTEX(name, initial_count) name,
#undef BEERTOS_TASK
#define BEERTOS_TASK(...)
#undef BEERTOS_ALARM_TASK
#define BEERTOS_ALARM_TASK(...)

#define BEERTOS_MUTEX_LIST() BEERTOS_PRIORITY_LIST()

/*! Mutex IDs - generated from BEERTOS_MUTEX_LIST() in BeeRTOS_mutex_cfg.h */
typedef enum 
{
    BEERTOS_MUTEX_LIST()
    BEERTOS_MUTEX_ID_MAX
} os_mutex_id_t; 

/******************************************************************************************
*                                    GLOBAL VARIABLES                                    *
******************************************************************************************/

/******************************************************************************************
*                                   FUNCTION PROTOTYPES                                  *
******************************************************************************************/
void os_mutex_module_init(void);
void os_mutex_unlock(const os_mutex_id_t id);
bool os_mutex_lock(const os_mutex_id_t id, const uint32_t timeout);

#endif /* __BEERTOS_MUTEX_H__ */