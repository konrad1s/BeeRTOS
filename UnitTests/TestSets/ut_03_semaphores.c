#include "ut_utils.h"

void TEST_couting_semaphores(void)
{
    bool ret;
    /* Test 1: wait for semaphore with timeout 0, SEMAPHORE_UT1_INITIAL_COUNT times
       Expected result: true */
    for (int i = 0; i < SEMAPHORE_UT1_INITIAL_COUNT; i++)
    {
        ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
        TEST_ASSERT(ret == true);
    }

    /* Test 2: wait for semaphore with timeout 0, one more time
       Expected result: false */
    ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
    TEST_ASSERT(ret == false);

     /* Test 2: wait for semaphore with timeout 1000, one more time
       Expected result: false and task blocked for 1000 ticks */
    uint32_t start = os_get_tick_count();
    ret = os_semaphore_wait(SEMAPHORE_UT1, 1000);
    uint32_t end = os_get_tick_count();
    TEST_ASSERT(ret == false);
    TEST_ASSERT(end - start == 1000);

    /* Test 3: signal semaphore
       Expected result: true */
    ret = os_semaphore_signal(SEMAPHORE_UT1);
    TEST_ASSERT(ret == true);

    /* Test 4: wait for semaphore with timeout 0, one more time
       Expected result: true */
    ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
    TEST_ASSERT(ret == true);
}

void TEST_binary_semaphores(void)
{
    bool ret;

    /* Test 1: wait for semaphore with timeout 0, 1 time
       Expected result: false */
    ret = os_semaphore_wait(SEMAPHORE_UT2, 0);
    TEST_ASSERT(ret == false);

    /* Test 2: signal semaphore
       Expected result: true */
    ret = os_semaphore_signal(SEMAPHORE_UT2);

    /* Test 3: signal semaphore
       Expected result: false */
    ret = os_semaphore_signal(SEMAPHORE_UT2);
    TEST_ASSERT(ret == false);

    /* Test 4: wait for semaphore with timeout 0, 1 time
       Expected result: true */
    ret = os_semaphore_wait(SEMAPHORE_UT2, 0);
    TEST_ASSERT(ret == true);
}

void TEST_semaphores(void)
{
    PRINT_UT_BEGIN();

    for (int i = 0; i < 10; i++)
    {
        os_semaphores_init();
        TEST_couting_semaphores();
        TEST_binary_semaphores();
    }
}