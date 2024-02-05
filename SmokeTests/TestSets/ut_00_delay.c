#include "ut_utils.h"

void TEST_delay(void)
{
    PRINT_UT_BEGIN();

    uint32_t start = os_get_tick_count();
    ut_blocking_delay(1549);
    uint32_t end = os_get_tick_count();
    TEST_ASSERT_EQUAL(1549, end - start);

    start = os_get_tick_count();
    ut_blocking_delay(1034);
    end = os_get_tick_count();
    TEST_ASSERT_EQUAL(1034, end - start);

    start = os_get_tick_count();
    ut_blocking_delay(0xA5);
    end = os_get_tick_count();
    TEST_ASSERT_EQUAL(0xA5, end - start);
}