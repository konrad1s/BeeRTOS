#include "ut_utils.h"

uint32_t tasks_cnt[3];

/* Lowest priority task */
void ut_task_mutex_1(void *arg)
{
    /* Lock the mutex */
    os_mutex_lock(MUTEX_ONE, 1000);
    ut_blocking_delay(20);

    /* priority should be inherited */
    ut_blocking_delay(80);
    os_mutex_unlock(MUTEX_ONE);

    tasks_cnt[0]++;
    os_task_delete();
}

/* Medium priority task */
void ut_task_mutex_2(void *arg)
{
    tasks_cnt[1]++;
    os_task_delete();
}

/* Highest priority task */
void ut_task_mutex_3(void *arg)
{
    /* Let the other tasks to lock the mutex */
    os_delay(5);

    tasks_cnt[2]++;

    os_mutex_lock(MUTEX_ONE, 2000);
    
    tasks_cnt[2]++;
    ut_blocking_delay(10);
    os_task_delete();
}

void TEST_mutexes(void)
{
    PRINT_UT_BEGIN();

    os_delay(300);

    TEST_ASSERT_EQUAL(1, tasks_cnt[0]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[1]);
    TEST_ASSERT_EQUAL(2, tasks_cnt[2]);
}