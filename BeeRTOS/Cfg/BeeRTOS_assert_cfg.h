/******************************************************************************************
 * @brief 
 * @file 
 * ****************************************************************************************/

#ifndef __BEERTOS_ASSERT_CFG_H__
#define __BEERTOS_ASSERT_CFG_H__

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include <stdint.h>

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define BEERTOS_USE_ASSERT_HISTORY_LOG   (true)
#define BEERTOS_ASSERT_HISTORY_LOG_SIZE  (10U)

#define BEERTOS_ASSERT_ERROR_HANDLER(module_id, error_id) \
    while (1);

#define BEERTOS_ASSERT_USER_LIST \
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