#include "unity.h"
#include "BeeRTOS.h"

static uint32_t tasks_cnt[3];

static void ut_task_block_delay(uint32_t delay)
{
    uint32_t start = os_get_tick_count();
    
    while(os_get_tick_count() - start < delay)
    {
        __asm("nop");
    }
}

void ut_task_priority_1(void)
{
    tasks_cnt[0]++;

    ut_task_block_delay(50);

    os_delay(50);

    os_task_delete();
}

void ut_task_priority_2(void)
{
    tasks_cnt[1]++;

    ut_task_block_delay(51);

    os_delay(50);

    os_task_delete();
}

void ut_task_priority_3(void)
{
    tasks_cnt[2]++;

    ut_task_block_delay(50);

    os_delay(25);
    tasks_cnt[2]++;

    os_delay(25);
    tasks_cnt[2]++;

    os_task_delete();
}

void TEST_Task(void)
{
    UnityPrint("TEST_Task\n");

    os_delay(101);

    TEST_ASSERT_EQUAL(0, tasks_cnt[0]);
    TEST_ASSERT_EQUAL(1, tasks_cnt[1]);
    TEST_ASSERT_EQUAL(3, tasks_cnt[2]);
}