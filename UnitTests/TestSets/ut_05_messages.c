#include "ut_utils.h"

void ut_task_msg_1(void *arg)
{
    uint8_t msg_received[8];
    uint8_t msg[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    os_message_receive(MESSAGE_ONE, msg_received, 1000);

    TEST_ASSERT_EQUAL(msg[0], msg_received[0]);
    TEST_ASSERT_EQUAL(msg[1], msg_received[1]);
    TEST_ASSERT_EQUAL(msg[2], msg_received[2]);
    TEST_ASSERT_EQUAL(msg[3], msg_received[3]);

    os_task_delete();
}

void ut_task_msg_2(void *arg)
{
    uint8_t msg[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    os_message_send(MESSAGE_ONE, msg, 1000);

    os_task_delete();
}

void TEST_messages(void)
{
    PRINT_UT_BEGIN();

    os_messages_init();

    os_task_start(OS_TASK_MSG_1);
    os_task_start(OS_TASK_MSG_2);

    os_delay(300);
}