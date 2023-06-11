#include "ut_utils.h"
#include "BeeRTOS_queue.h"

extern void os_queue_reset(os_queue_id_t id);
extern void os_queues_init(void);
extern bool os_queue_full(os_queue_id_t id);
extern bool os_queue_push(os_queue_id_t id, void *data, uint32_t len);
extern bool os_queue_empty(os_queue_id_t id);
extern bool os_queue_pop(os_queue_id_t id, void *data, uint32_t len);

void TEST_queues(void)
{
    PRINT_UT_BEGIN();

    os_queues_init();

    uint8_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t received[10];

    TEST_ASSERT_EQUAL(false, os_queue_full(QUEUE_1));
    TEST_ASSERT_EQUAL(true, os_queue_empty(QUEUE_1));

    TEST_ASSERT_EQUAL(true, os_queue_push(QUEUE_1, data, 10));
    TEST_ASSERT_EQUAL(true, os_queue_full(QUEUE_1));

    TEST_ASSERT_EQUAL(false, os_queue_push(QUEUE_1, data, 10));
    TEST_ASSERT_EQUAL(true, os_queue_full(QUEUE_1));

    TEST_ASSERT_EQUAL(true, os_queue_pop(QUEUE_1, received, 10));
    TEST_ASSERT_EQUAL(true, os_queue_empty(QUEUE_1));

    TEST_ASSERT_EQUAL(false, os_queue_pop(QUEUE_1, received, 10));
    TEST_ASSERT_EQUAL(true, os_queue_empty(QUEUE_1));

    for (uint32_t i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL(data[i], received[i]);
    }
}