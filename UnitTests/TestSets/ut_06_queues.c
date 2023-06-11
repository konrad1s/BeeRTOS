#include "ut_utils.h"

void TEST_queues(void)
{
    PRINT_UT_BEGIN();

    os_queues_init();

    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t received[10];

    TEST_ASSERT_EQUAL(false, os_queue_is_full(QUEUE_1));
    TEST_ASSERT_EQUAL(true, os_queue_is_empty(QUEUE_1));

    TEST_ASSERT_EQUAL(true, os_queue_push(QUEUE_1, data, 10));
    TEST_ASSERT_EQUAL(true, os_queue_is_full(QUEUE_1));

    TEST_ASSERT_EQUAL(false, os_queue_push(QUEUE_1, data, 10));
    TEST_ASSERT_EQUAL(true, os_queue_is_full(QUEUE_1));

    TEST_ASSERT_EQUAL(true, os_queue_pop(QUEUE_1, received, 10));
    TEST_ASSERT_EQUAL(true, os_queue_is_empty(QUEUE_1));

    TEST_ASSERT_EQUAL(false, os_queue_pop(QUEUE_1, received, 10));
    TEST_ASSERT_EQUAL(true, os_queue_is_empty(QUEUE_1));

    for (uint32_t i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL(data[i], received[i]);
    }
}