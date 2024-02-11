#include "ut_utils.h"

void fill_test_data(uint8_t *data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = i + 1;
    }
}

bool compare_arrays(const uint8_t *arr1, const uint8_t *arr2, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}

void TEST_queue_basic_push_pop(void)
{
    uint8_t data[10];
    uint8_t received[10];
    fill_test_data(data, sizeof(data));

    os_queue_reset(QUEUE_1);

    bool push_result = os_queue_push(QUEUE_1, data, sizeof(data));
    TEST_ASSERT_TRUE(push_result);

    bool pop_result = os_queue_pop(QUEUE_1, received, sizeof(received));
    TEST_ASSERT_TRUE(pop_result);

    bool data_match = compare_arrays(data, received, sizeof(data));
    TEST_ASSERT_TRUE(data_match);
}

void TEST_queue_overflow(void)
{
    uint8_t data[10];
    fill_test_data(data, sizeof(data));

    os_queue_reset(QUEUE_1);

    bool push_result = true;
    while (push_result)
    {
        push_result = os_queue_push(QUEUE_1, data, sizeof(data));
    }

    TEST_ASSERT_FALSE(push_result);
    TEST_ASSERT_TRUE(os_queue_is_full(QUEUE_1));
}

void TEST_queue_underflow(void)
{
    uint8_t received[10];

    os_queue_reset(QUEUE_1);

    bool pop_result = os_queue_pop(QUEUE_1, received, sizeof(received));

    TEST_ASSERT_FALSE(pop_result);
    TEST_ASSERT_TRUE(os_queue_is_empty(QUEUE_1));
}

void TEST_queues(void)
{
    PRINT_UT_BEGIN();

    TEST_queue_basic_push_pop();
    TEST_queue_overflow();
    TEST_queue_underflow();
}
