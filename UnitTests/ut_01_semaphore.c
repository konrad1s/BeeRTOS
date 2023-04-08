#include "unity.h"
#include "BeeRTOS.h"
#include "BeeRTOS_semaphore.h"

void TEST_RecursiveMutex(void) 
{
    UnityPrint("TEST_RecursiveMutex\n");
    os_sem_t *test_sem;

     for (int i = 0; i < 10; i++)
    {

        for (int i = 0; i < 10; i++)
        {
            // uint32_t start = task_test_sem.ticks;

            os_semaphore_wait(SEMAPHORE_UT, 1000);
            // uint32_t end = task_test_sem.ticks;
            // TEST_ASSERT_TRUE(end - start == 1000);
        }

        os_semaphore_get_control_block_info(SEMAPHORE_UT, &test_sem);
        TEST_ASSERT_EQUAL(10, test_sem->count);

        for (int i = 0; i < 10; i++)
        {
            os_semaphore_signal(SEMAPHORE_UT);
        }

        TEST_ASSERT_EQUAL(0, test_sem->count);
    }
}