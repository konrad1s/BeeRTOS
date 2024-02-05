#include "ut_utils.h"

static uint32_t start_tick, end_tick;

static void reset_tick_counter(void)
{
    start_tick = os_get_tick_count();
}

static uint32_t get_elapsed_ticks(void)
{
    end_tick = os_get_tick_count();
    return end_tick - start_tick;
}

void TEST_counting_semaphores(void)
{
    bool ret;

    /* Test semaphore availability and exhaustion */
    for (int i = 0; i < 10; i++)
    {
        ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
        TEST_ASSERT_TRUE_MESSAGE(ret, "Semaphore should be available.");
    }

    /* Test semaphore unavailability after exhaustion */
    ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Semaphore should be unavailable after 10 waits.");

    /* Test semaphore wait with timeout, expecting timeout */
    reset_tick_counter();
    ret = os_semaphore_wait(SEMAPHORE_UT1, 1000);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Semaphore wait should timeout.");
    TEST_ASSERT_EQUAL_MESSAGE(1000, get_elapsed_ticks(), "Task should be blocked for 1000 ticks.");

    /* Test signaling semaphore to make it available again */
    ret = os_semaphore_signal(SEMAPHORE_UT1);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Semaphore signal should succeed.");

    /* Test semaphore availability after signaling */
    ret = os_semaphore_wait(SEMAPHORE_UT1, 0);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Semaphore should be available after signaling.");
}

void TEST_binary_semaphores(void)
{
    bool ret;

    /* Test initial semaphore wait with no signal, expecting failure */
    ret = os_semaphore_wait(SEMAPHORE_UT2, 0);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Initial semaphore wait should fail without signal.");

    /* Test signaling semaphore to make it available */
    ret = os_semaphore_signal(SEMAPHORE_UT2);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Semaphore signal should succeed.");

    /* Test semaphore availability after signaling */
    ret = os_semaphore_wait(SEMAPHORE_UT2, 0);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Semaphore should be available after signaling.");

    /* Test signaling semaphore again, expecting failure for binary semaphore */
    (void)os_semaphore_signal(SEMAPHORE_UT2);
    ret = os_semaphore_signal(SEMAPHORE_UT2);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Double signaling of binary semaphore should fail.");

    /* Test semaphore unavailability after wait */
    (void)os_semaphore_wait(SEMAPHORE_UT2, 0);
    ret = os_semaphore_wait(SEMAPHORE_UT2, 0);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Semaphore should be unavailable after wait.");
}

void TEST_semaphores(void)
{
    PRINT_UT_BEGIN();

    TEST_counting_semaphores();
    TEST_binary_semaphores();
}