#include "ut_utils.h"

void TEST_kernel(void)
{
    PRINT_UT_BEGIN();

    uint32_t start = os_get_tick_count();

    ut_blocking_delay(1549);

    uint32_t end = os_get_tick_count();

    TEST_ASSERT_EQUAL(1549, end - start);
}