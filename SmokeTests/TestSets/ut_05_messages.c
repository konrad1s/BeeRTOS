#include "ut_utils.h"

void ut_task_msg_1(void *arg)
{
    uint8_t msg_received[8];
    bool ret;

    ret = os_message_receive(MESSAGE_ONE, msg_received, 0);
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(msg_received, "ABCDEFGH", 8, "Received message does not match expected.");

    /* -- Contex switch -- */

    ret = os_message_receive(MESSAGE_ONE, msg_received, 0);
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(msg_received, "12345678", 8, "Received message does not match expected.");

    ret = os_message_receive(MESSAGE_ONE, msg_received, 0);
    TEST_ASSERT_EQUAL(true, ret);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(msg_received, "TIMEOUT", 8, "Received message does not match expected.");

    os_task_delete();
}

void ut_task_msg_2(void *arg)
{
    bool ret;

    uint8_t msg1[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    ret = os_message_send(MESSAGE_ONE, msg1, 0);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Message should be sent successfully when queue is not full.");

    uint8_t msg2[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    ret = os_message_send(MESSAGE_ONE, msg2, 0);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Message should be sent successfully when queue is not full.");

    uint8_t msgTimeout[] = {'T', 'I', 'M', 'E', 'O', 'U', 'T'};
    ret = os_message_send(MESSAGE_ONE, msgTimeout, 1000);
    TEST_ASSERT_TRUE_MESSAGE(ret, "Message sending should be successful if timeout is not reached.");

    /* Attempt to send one more message should result in failure due to overflow */
    uint8_t msg_overflow[] = {'O', 'V', 'E', 'R', 'F', 'L', 'O', 'W'};
    ret = os_message_send(MESSAGE_ONE, msg_overflow, 0);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Message should be sent successfully until the queue is full.");

    /* Empty the message queue in the ut_task_msg_1 */
    os_delay(10);

    /* Attempt to receive one more message should result in failure due to empty queue */
    uint8_t msg_received[8];
    ret = os_message_receive(MESSAGE_ONE, msg_received, 0);
    TEST_ASSERT_FALSE_MESSAGE(ret, "Message receiving should fail due to queue underflow.");

    os_task_delete();
}

void TEST_messages(void)
{
    PRINT_UT_BEGIN();

    os_task_start(OS_TASK_MSG_1);
    os_task_start(OS_TASK_MSG_2);

    os_delay(300);
}
