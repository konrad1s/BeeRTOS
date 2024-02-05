#include "ut_utils.h"
#include <string.h>

static uint32_t alarms_cnt[BEERTOS_ALARM_ID_MAX];

void alarm1_callback(void) { alarms_cnt[ALARM_ONE]++; }
void alarm2_callback(void) { alarms_cnt[ALARM_TWO]++; }
void alarm3_callback(void) { alarms_cnt[ALARM_THREE]++; }

void reset_alarm_counters(void)
{
    memset(alarms_cnt, 0, sizeof(alarms_cnt));
}

void setup_alarms_for_test(void)
{
    reset_alarm_counters();
    /* Non-periodic alarm, 10 ticks */
    os_alarm_start(ALARM_ONE, 10, false);
    /* Periodic alarm, 20 ticks */
    os_alarm_start(ALARM_TWO, 20, true);
    /* Non-periodic alarm, 30 ticks */
    os_alarm_start(ALARM_THREE, 30, false);
}

void TEST_basic_alarm_timing(void)
{
    setup_alarms_for_test();

    /* After 11 ticks, only ALARM_ONE should have triggered */
    os_delay(11);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_THREE]);

    /* After another 11 ticks, ALARM_TWO should have triggered once */
    os_delay(11);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(0, alarms_cnt[ALARM_THREE]);

    /* After 21 more ticks, ALARM_TWO should have triggered again and ALARM_THREE once */
    os_delay(21);
    TEST_ASSERT_EQUAL(2, alarms_cnt[ALARM_TWO]);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);
}

void TEST_periodic_alarm_behavior(void)
{
    setup_alarms_for_test();

    /* Fast forward time to observe periodic behavior */
    os_delay(100);
    /* ALARM_ONE should have triggered once, ALARM_TWO 4 times, ALARM_THREE once */
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_ONE]);
    TEST_ASSERT_EQUAL(alarms_cnt[ALARM_TWO], 4);
    TEST_ASSERT_EQUAL(1, alarms_cnt[ALARM_THREE]);

    /* Cancel ALARM_TWO and observe no further increments */
    os_alarm_cancel(ALARM_TWO);
    uint32_t alarm_two_count = alarms_cnt[ALARM_TWO];
    os_delay(100);
    TEST_ASSERT_EQUAL(alarm_two_count, alarms_cnt[ALARM_TWO]);
}

void TEST_restart_canceled_alarm(void)
{
    reset_alarm_counters();
    os_alarm_start(ALARM_TWO, 20, true);

    /* Allow some time for ALARM_TWO to trigger a few times */
    os_delay(101);
    TEST_ASSERT_TRUE(alarms_cnt[ALARM_TWO] > 0);

    /* Cancel and immediately restart ALARM_TWO */
    os_alarm_cancel(ALARM_TWO);
    os_alarm_start(ALARM_TWO, 20, true);

    /* Verify it continues to trigger after restart */
    uint32_t count_before_delay = alarms_cnt[ALARM_TWO];
    /* Wait for at least two more periods */
    os_delay(41);
    TEST_ASSERT_TRUE(alarms_cnt[ALARM_TWO] > count_before_delay);

    os_alarm_cancel(ALARM_TWO);
}

void TEST_alarms(void)
{
    PRINT_UT_BEGIN();
    TEST_basic_alarm_timing();
    TEST_periodic_alarm_behavior();
    TEST_restart_canceled_alarm();
}