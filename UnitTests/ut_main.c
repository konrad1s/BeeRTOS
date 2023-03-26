#include "unity.h"
#include "BeeRTOS.h"

extern void TEST_RecursiveMutex(void);
extern void TEST_Alarms(void);

void ut_beertos_main_task(void)
{
    UnityBegin("ut_00_semaphore.c");

    RUN_TEST(TEST_RecursiveMutex);
    RUN_TEST(TEST_Alarms);

    UnityEnd();

    os_task_delete();

    while(1)
    {

    }
}