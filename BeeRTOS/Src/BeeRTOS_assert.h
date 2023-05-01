/******************************************************************************************
 * @brief 
 * @file 
 * ****************************************************************************************/

#ifndef __BEERTOS_ASSERT_H__
#define __BEERTOS_ASSERT_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_internal.h"
#include "BeeRTOS_assert_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define BEERTOS_ASSERT(expr, module_id, error_id)   \
    if (!(expr))                                    \
    {                                               \
        os_report_error(module_id, error_id);       \
    }

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef BEERTOS_ASSERT_USER
#define BEERTOS_ASSERT_USER(module_id, ...)  module_id,

typedef enum
{
    OS_MODULE_ID_KERNEL,
    OS_MODULE_ID_TASK,
    OS_MODULE_ID_ALARM,
    OS_MODULE_ID_MUTEX,
    OS_MODULE_ID_SEMAPHORE,
    OS_MODULE_ID_QUEUE,

    BEERTOS_ASSERT_USER_LIST

    OS_MODULE_ID_MAX
} os_assert_module_id_t;

#undef BEERTOS_ASSERT_USER
#define BEERTOS_ASSERT_USER(module_id, ...)  __VA_ARGS__,

typedef enum
{
    OS_ERROR_NOT_INITIALIZED,
    OS_ERROR_INVALID_PARAM,
    OS_ERROR_INVALID_STATE,
    OS_ERROR_INVALID_OPERATION,
    OS_ERROR_NO_MEMORY,
    OS_ERROR_NO_RESOURCE,
    OS_ERROR_NO_SPACE,
    OS_ERROR_NO_DATA,
    OS_ERROR_TIMEOUT,
    OS_ERROR_OVERFLOW,
    OS_ERROR_NULLPTR,

    BEERTOS_ASSERT_USER_LIST

    OS_ERROR_ID_MAX
} os_assert_error_id_t;

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

void os_report_error(uint8_t module_id, uint8_t error_id);

#endif /* __BEERTOS_ASSERT_H__ */