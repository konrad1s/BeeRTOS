#include "ut_utils.h"

static uint32_t alarms_cnt[BEERTOS_ALARM_ID_MAX];

void alarm1_callback(void)
{
    alarms_cnt[ALARM_ONE]++;
}

void alarm2_callback(void)
{
    alarms_cnt[ALARM_TWO]++;
}

void alarm3_callback(void)
{
    alarms_cnt[ALARM_THREE]++;
}

void TEST_alarms(void)
{
    PRINT_UT_BEGIN();

    os_alarm_start(ALARM_ONE, 10, false);
    os_alarm_start(ALARM_TWO, 20, true);
    os_alarm_start(ALARM_THREE, 30, false);

    os_delay(11);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_THREE]);

    os_delay(11);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_THREE]);

    os_delay(21);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(2, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    os_delay(100);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(7, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    os_alarm_cancel(ALARM_TWO);

    os_delay(100);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(7, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    os_alarm_start(ALARM_TWO, 20, true);
    os_delay(101);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(12, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    os_alarm_cancel(ALARM_TWO);
}