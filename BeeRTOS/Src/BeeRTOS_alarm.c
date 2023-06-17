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

#define OS_ALARM_ENABLE(alarm_id)   (os_alarm_active_mask |= (1U << alarm_id))
#define OS_ALARM_DISABLE(alarm_id)  (os_alarm_active_mask &= ~(1U << alarm_id))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#if (BEERTOS_ALARM_ID_MAX <= 8U)
    typedef uint8_t os_alarm_active_mask_t;
#elif (BEERTOS_ALARM_ID_MAX <= 16U)
    typedef uint16_t os_alarm_active_mask_t;
#elif (BEERTOS_ALARM_ID_MAX <= 32U)
    typedef uint32_t os_alarm_active_mask_t;
#elif (BEERTOS_ALARM_ID_MAX <= 64U)
    typedef uint64_t os_alarm_active_mask_t;
#else
    #error "BEERTOS_ALARM_ID_MAX must be less or equal to 64"
#endif

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

static os_alarm_t alarms[BEERTOS_ALARM_ID_MAX];
static uint32_t ticks[BEERTOS_ALARM_ID_MAX];

static os_alarm_active_mask_t os_alarm_active_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_alarm_init(void)
{
    os_alarm_active_mask = 0U;

    #undef BEERTOS_ALARM
    #define BEERTOS_ALARM(name, _callback, _autostart, _period, _periodic) \
        alarms[name].period = _period; \
        alarms[name].periodic = _periodic; \
        alarms[name].callback = _callback; \
        ticks[name] = 0U; \
        if (true == _autostart) \
        { \
            os_alarm_start(name, _period, _periodic); \
        }
    
    #define BEERTOS_ALARMS_INIT_ALL() BEERTOS_ALARM_LIST()

    BEERTOS_ALARMS_INIT_ALL();
}

void os_alarm_start(os_alarm_id_t alarm_id, uint32_t period, bool periodic)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    alarms[alarm_id].period = period;
    alarms[alarm_id].periodic = periodic;
    ticks[alarm_id] = period;
    OS_ALARM_ENABLE(alarm_id);
}

void os_alarm_cancel(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    ticks[alarm_id] = 0U;
    OS_ALARM_DISABLE(alarm_id);
}

void os_alarm_tick(void)
{
    if (0U != os_alarm_active_mask)
    {
        for (os_alarm_id_t i = 0U; i < BEERTOS_ALARM_ID_MAX; i++)
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