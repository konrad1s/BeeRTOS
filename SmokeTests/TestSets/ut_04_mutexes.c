#include "ut_utils.h"

enum
{
    UT_TASK_PRIORITY_LOWEST = 0,
    UT_TASK_PRIORITY_HIGHEST,
    UT_TASK_PRIORITY_COUNT
} ut_mutex_tasks_priority_t;

volatile uint32_t tasks_cnt[UT_TASK_PRIORITY_COUNT];

void reset_counters(void)
{
    for (int i = 0; i < 3; i++)
    {
        tasks_cnt[i] = 0;
    }
}

void mutex_low_priority_task(void *arg)
{
    tasks_cnt[UT_TASK_PRIORITY_LOWEST]++;

    os_mutex_lock(MUTEX_ONE, 10000);
    tasks_cnt[UT_TASK_PRIORITY_LOWEST]++;
    ut_blocking_delay(20);
    os_mutex_unlock(MUTEX_ONE);

    tasks_cnt[UT_TASK_PRIORITY_LOWEST]++;

    while(1)
    {
        os_delay(500);
    }
}

void mutex_high_priority_task(void *arg)
{
    /* Let the lowest priority task start first */
    os_delay(5);

    /* This task should be blocked by the lowest priority task due
     * to the priority ceiling of the mutex */
    tasks_cnt[UT_TASK_PRIORITY_HIGHEST]++;

    os_mutex_lock(MUTEX_ONE, 2000);
    tasks_cnt[UT_TASK_PRIORITY_HIGHEST]++;
    ut_blocking_delay(10);

    while(1)
    {
        os_delay(500);
    }
}

void TEST_mutexes(void)
{
    PRINT_UT_BEGIN();

    reset_counters();
    os_task_start(OS_TASK_MUTEX_3);
    os_task_start(OS_TASK_MUTEX_1);

    os_delay(10);
    /* Only the lowest priority task should have executed due to the mutex priority ceiling */
    TEST_ASSERT_EQUAL(2, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(0, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);
    os_delay(15);
    /* The highest priority task should have executed after the lowest priority task unlocked the mutex */
    TEST_ASSERT_EQUAL(2, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(2, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);
    os_delay(25);
    /* The lowest priority task should have executed again after the highest priority task unlocked the mutex */
    TEST_ASSERT_EQUAL(3, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(2, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);
}
