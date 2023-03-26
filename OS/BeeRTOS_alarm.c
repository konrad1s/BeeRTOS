/******************************************************************************************
 * @brief 
 * @file h_template.h
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_alarm.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#undef BEERTOS_ALARM
#define BEERTOS_ALARM(name, callback) \
    /* period, periodic, callback */  \
    { 0,       false,    callback },

static alarm_t alarms[] = {
    BEERTOS_ALARM_LIST
};

static uint32_t ticks[ALARM_ID_MAX];
static uint32_t os_alarm_active_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_alarms_init(void)
{
    alarm_id_t i;

    for (i = 0U; i < ALARM_ID_MAX; i++)
    {
        alarms[i].period = 0U;
        alarms[i].periodic = false;
        ticks[i] = 0U;
    }
    os_alarm_active_mask = 0U;
}

void os_alarm_start(alarm_id_t alarm_id, uint32_t period, bool periodic)
{
    if (alarm_id < ALARM_ID_MAX)
    {
        alarms[alarm_id].period = period;
        alarms[alarm_id].periodic = periodic;
        ticks[alarm_id] = period;
        os_alarm_active_mask |= (1U << alarm_id);
    }
}

void os_alarm_cancel(alarm_id_t alarm_id)
{
    if (alarm_id < ALARM_ID_MAX)
    {
        ticks[alarm_id] = 0U;
        os_alarm_active_mask &= ~(1U << alarm_id);
    }
}

void os_alarm_tick(void)
{
    if (0U != os_alarm_active_mask)
    {
        for (alarm_id_t i = 0U; i < ALARM_ID_MAX; i++)
        {
            if (ticks[i] > 0U && 0U != (os_alarm_active_mask & (1U << i)))
            {
                ticks[i]--;

                if (0U == ticks[i] && NULL != alarms[i].callback)
                {
                    alarms[i].callback();

                    if (true == alarms[i].periodic)
                    {
                        ticks[i] = alarms[i].period;
                    }
                    else
                    {
                        os_alarm_active_mask &= ~(1U << i);
                    }
                }
            }
        }
    }
}