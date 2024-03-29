#include "unity.h"
#include "BeeRTOS.h"

extern void TEST_delay(void);
extern void TEST_rtos_task_core(void);
extern void TEST_semaphores(void);
extern void TEST_alarms(void);
extern void TEST_mutexes(void);
extern void TEST_messages(void);
extern void TEST_queues(void);

void (*test_functions[])(void) = {
    TEST_delay,
    TEST_rtos_task_core,
    TEST_alarms,
    TEST_semaphores,
    TEST_mutexes,
    TEST_messages,
    TEST_queues,
};

void ut_beertos_main_task(void *arg)
{
    UnityBegin("BeeRTOS Unit Tests");

    for (int i = 0; i < sizeof(test_functions) / sizeof(test_functions[0]); i++)
    {
        RUN_TEST(test_functions[i]);
    }

    UnityEnd();

    os_task_delete();

    while(1)
    {

    }
}