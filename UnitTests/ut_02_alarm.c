#include "BeeRTOS_alarm.h"
#include "unity.h"

uint32_t alarms_cnt[ALARM_ID_MAX];

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

void TEST_Alarms(void)
{
    UnityPrint("TEST_Alarms\n");

    os_alarms_init();

    os_alarm_start(ALARM_ONE, 10, false);
    os_alarm_start(ALARM_TWO, 20, true);
    os_alarm_start(ALARM_THREE, 30, false);

    os_delay(101);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(5, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    os_alarm_cancel(ALARM_TWO);

    os_delay(101);

    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(5, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);
}