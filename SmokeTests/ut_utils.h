#ifndef __UT_UTILS_H__
#define __UT_UTILS_H__

#include "BeeRTOS.h"
#include "unity.h"

#define PRINT_UT_BEGIN()    UnityPrint("Running TEST - ");  \
                            UnityPrint(__FUNCTION__);       \
                            UnityPrint("\n");

void ut_blocking_delay(uint32_t delay);

#endif // __UT_UTILS_H__