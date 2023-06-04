#include "ut_utils.h"

void TEST_semaphores(void)
{
    PRINT_UT_BEGIN();

    /* Test 1: wait for semaphore with timeout 0, SEMAPHORE_UT_INITIAL_COUNT times
       Expected result: true */
    for (int i = 0; i < SEMAPHORE_UT_INITIAL_COUNT; i++)
    {
        bool ret = os_semaphore_wait(SEMAPHORE_UT, 0);
        TEST_ASSERT(ret == true);
    }

    /* Test 2: wait for semaphore with timeout 0, one more time
       Expected result: false */
    bool ret = os_semaphore_wait(SEMAPHORE_UT, 0);
    TEST_ASSERT(ret == false);

     /* Test 2: wait for semaphore with timeout 1000, one more time
       Expected result: false and task blocked for 1000 ticks */
    uint32_t start = os_get_tick_count();
    ret = os_semaphore_wait(SEMAPHORE_UT, 1000);
    uint32_t end = os_get_tick_count();
    TEST_ASSERT(ret == false);
    TEST_ASSERT(end - start == 1000);

    /* Test 3: signal semaphore
       Expected result: true */
    ret = os_semaphore_signal(SEMAPHORE_UT);
    TEST_ASSERT(ret == true);

    /* Test 4: wait for semaphore with timeout 0, one more time
       Expected result: true */
    ret = os_semaphore_wait(SEMAPHORE_UT, 0);
    TEST_ASSERT(ret == true);
}