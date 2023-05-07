/******************************************************************************************
 * @brief OS alarm source file
 * @file BeeRTOS_alarm.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_alarm.h"
#include "BeeRTOS_assert.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#if ALARM_ID_MAX > 32U
    #error "ALARM_ID_MAX must be less than or equal to 32"
#endif

#define OS_ALARM_ENABLE(alarm_id)   (os_alarm_active_mask |= (1U << alarm_id))
#define OS_ALARM_DISABLE(alarm_id)  (os_alarm_active_mask &= ~(1U << alarm_id))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#if (ALARM_ID_MAX <= 8U)
    typedef uint8_t os_alarm_active_mask_t;
#elif (ALARM_ID_MAX <= 16U)
    typedef uint16_t os_alarm_active_mask_t;
#else
    typedef uint32_t os_alarm_active_mask_t;
#endif

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

#undef BEERTOS_ALARM
#define BEERTOS_ALARM(name, callback) \
    /* period, periodic, callback */  \
    { 0,       false,    callback },

static os_alarm_t alarms[ALARM_ID_MAX] = {
    BEERTOS_ALARM_LIST
};

static uint32_t ticks[ALARM_ID_MAX];
static os_alarm_active_mask_t os_alarm_active_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_alarms_init(void)
{
    for (os_alarm_id_t i = 0U; i < ALARM_ID_MAX; i++)
    {
        alarms[i].period = 0U;
        alarms[i].periodic = false;
        ticks[i] = 0U;
    }
    os_alarm_active_mask = 0U;
}

void os_alarm_start(os_alarm_id_t alarm_id, uint32_t period, bool periodic)
{
    BEERTOS_ASSERT(alarm_id < ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    alarms[alarm_id].period = period;
    alarms[alarm_id].periodic = periodic;
    ticks[alarm_id] = period;
    OS_ALARM_ENABLE(alarm_id);
}

void os_alarm_cancel(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    ticks[alarm_id] = 0U;
    OS_ALARM_DISABLE(alarm_id);
}

void os_alarm_tick(void)
{
    if (0U != os_alarm_active_mask)
    {
        for (os_alarm_id_t i = 0U; i < ALARM_ID_MAX; i++)
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
                        OS_ALARM_DISABLE(i);
                    }
                }
            }
        }
    }
}