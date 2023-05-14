#include "ut_utils.h"

enum
{
    UT_TASK_PRIORITY_LOWEST = 0,
    UT_TASK_PRIORITY_MEDIUM,
    UT_TASK_PRIORITY_HIGHEST,
};

static uint32_t tasks_cnt[3] = {0};

void ut_task_priority_lowest(void *arg)
{
    tasks_cnt[UT_TASK_PRIORITY_LOWEST]++;

    ut_blocking_delay(50);

    os_delay(50);

    os_task_delete();
}

void ut_task_priority_medium(void *arg)
{
    tasks_cnt[UT_TASK_PRIORITY_MEDIUM]++;

    ut_blocking_delay(51);

    os_delay(31);

    os_task_delete();
}

void ut_task_priority_highest(void *arg)
{
    tasks_cnt[UT_TASK_PRIORITY_HIGHEST]++;

    ut_blocking_delay(50);

    os_delay(25);
    tasks_cnt[UT_TASK_PRIORITY_HIGHEST]++;

    os_delay(25);
    tasks_cnt[UT_TASK_PRIORITY_HIGHEST]++;

    os_task_delete();
}

void TEST_rtos_task_core(void)
{
    PRINT_UT_BEGIN();

    os_delay(20);
    TEST_ASSERT_EQUAL(0, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(0, tasks_cnt[UT_TASK_PRIORITY_MEDIUM]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);

    os_delay(31);
    TEST_ASSERT_EQUAL(0, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_MEDIUM]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);

    os_delay(26);
    TEST_ASSERT_EQUAL(0, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_MEDIUM]);
    TEST_ASSERT_EQUAL(2, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);

    os_delay(50);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_LOWEST]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[UT_TASK_PRIORITY_MEDIUM]);
    TEST_ASSERT_EQUAL(3, tasks_cnt[UT_TASK_PRIORITY_HIGHEST]);

    os_delay(100);
}