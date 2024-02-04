/******************************************************************************************
 * @brief Configuration Header for BeeRTOS Assertions
 * @file BeeRTOS_assert_cfg.h
 * This configuration header file allows for customization of the BeeRTOS assertion mechanism,
 * including enabling an assertion history log to track recent assertion failures and defining
 * a custom error handler for assertions. It supports the definition of application-specific
 * error codes and module IDs to extend the assertion functionality beyond the OS internals,
 * facilitating debugging and system integrity verification in both development and production
 * environments.
 ******************************************************************************************/

#ifndef __BEERTOS_ASSERT_CFG_H__
#define __BEERTOS_ASSERT_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include <stdint.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/*! @brief Enable assertion history log
 * If enabled, the assertion history log will be used to store the last N assertions.
 * The size of the log is defined by BEERTOS_ASSERT_HISTORY_LOG_SIZE.
 * The log can be read using os_assert_get_history_log() function.
 */
#define BEERTOS_USE_ASSERT_HISTORY_LOG   (true)
#define BEERTOS_ASSERT_HISTORY_LOG_SIZE  (10U)

/*! @brief User defined error handler
 * Error handler called when an assertion is triggered.
 * 
 * Structure BEERTOS_ASSERT_ERROR_HANDLER(module_id, error_id)
 * @param module_id - module id of the assertion
 * @param error_id - error id of the assertion
 */
#define BEERTOS_ASSERT_ERROR_HANDLER(module_id, error_id) \
    while (1);

/*! @brief User defined error codes
 * Assertions can be used to report errors that are related to the os module.
 * User can define their own error codes that are related to the application.
 * If the assertion is triggered, the error handler will be called with the module id and error id.
 * The error handler can be defined in the application using BEERTOS_ASSERT_ERROR_HANDLER(module_id, error_id) macro.
 * 
 * Structure BEERTOS_ASSERT_USER:
 * BEERTOS_ASSERT_USER(module_id, error_id_1, ...)
 * @param module_id - module id (created in os_assert_module_id_t enum), must be unique
 * @param error_id_X - error id (created in os_assert_error_id_t enum), must be unique
 * @params ... - other error ids
 */
#define BEERTOS_ASSERT_USER_LIST() \
    /* module id, list of error ids */ \
    BEERTOS_ASSERT_USER(OS_ASSERT_USER1, OS_USER_ERROR_1, OS_USER_ERROR_2, OS_USER_ERROR_3) \
    BEERTOS_ASSERT_USER(OS_ASSERT_USER2, OS_USER_ERROR_10, OS_USER_ERROR_11, OS_USER_ERROR_12, OS_USER_ERROR_13)

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                    GLOBAL VARIABLES                                    *
 ******************************************************************************************/

/******************************************************************************************
 *                                   FUNCTION PROTOTYPES                                  *
 ******************************************************************************************/

#endif /*__BEERTOS_ASSERT_CFG_H__*/