/******************************************************************************************
 * @brief OS Assertion and Error Handling Source File
 * @file BeeRTOS_assert.c
 * This file implements the assertion and error handling mechanisms for BeeRTOS. It provides
 * functionalities to report errors encountered within the operating system components and, if
 * enabled, log a history of recent errors for debugging purposes. The implementation includes
 * storing error details such as the module ID and error ID, and optionally, the system tick count
 * when the error occurred. It leverages the BEERTOS_ASSERT macro for error detection and the
 * BEERTOS_ASSERT_ERROR_HANDLER for customizable error handling strategies.
 ******************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_assert.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#if (BEERTOS_USE_ASSERT_HISTORY_LOG == true)
static os_error_t errors[BEERTOS_ASSERT_HISTORY_LOG_SIZE];
#endif

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/

/**
 * @brief The function is called when an error occurs in the OS, by the BEERTOS_ASSERT macro.
 * If BEERTOS_USE_ASSERT_HISTORY_LOG is set to true, the function stores the error in the
 * history log. The function then calls BEERTOS_ASSERT_ERROR_HANDLER macro, which is defined
 * in BeeRTOS_assert_cfg.h
 *
 * @param module_id Module ID of the module where the error occurred
 * @param error_id Error ID of the error that occurred
 * @return None
 */
void os_report_error(const uint8_t module_id, const uint8_t error_id)
{
#if (BEERTOS_USE_ASSERT_HISTORY_LOG == true)
    static uint8_t index = 0U;
    errors[index].module_id = module_id;
    errors[index].error_id = error_id;
    errors[index].timestamp = os_get_tick_count();
    index = (index + 1U) % BEERTOS_ASSERT_HISTORY_LOG_SIZE;
#endif

    BEERTOS_ASSERT_ERROR_HANDLER(module_id, error_id);
}

/**
 * @brief The function returns the last BEERTOS_ASSERT_HISTORY_LOG_SIZE errors that occurred
 * in the OS. The function is only available if BEERTOS_USE_ASSERT_HISTORY_LOG is set to true,
 * otherwise it returns NULL.
 *
 * @param None
 * @return Pointer to the history log
 */
os_error_t* os_assert_get_history_log(void)
{
#if (BEERTOS_USE_ASSERT_HISTORY_LOG == true)
    return errors;
#else
    return NULL;
#endif
}