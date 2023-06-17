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

typedef struct
{
    uint32_t    period;
    bool        periodic;
    void        (*callback)(void);
    uint32_t    remaining_time;
} os_alarm_t;

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

static os_alarm_t os_alarms[BEERTOS_ALARM_ID_MAX];
static os_alarm_active_mask_t os_alarm_active_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_alarm_init(void)
{
    os_alarm_active_mask = 0U;

    #undef BEERTOS_ALARM
    #define BEERTOS_ALARM(name, _callback, _autostart, _period, _periodic) \
        BEERTOS_ASSERT(_callback != NULL, OS_MODULE_ID_ALARM, OS_ERROR_NULLPTR); \
        os_alarms[name].period = _period; \
        os_alarms[name].periodic = _periodic; \
        os_alarms[name].callback = _callback; \
        os_alarms[name].remaining_time = 0U; \
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

    os_alarms[alarm_id].period = period;
    os_alarms[alarm_id].periodic = periodic;
    os_alarms[alarm_id].remaining_time = period;
    OS_ALARM_ENABLE(alarm_id);
}

void os_alarm_cancel(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    os_alarms[alarm_id].remaining_time = 0U;
    OS_ALARM_DISABLE(alarm_id);
}

void os_alarm_tick(void)
{
    if (0U != os_alarm_active_mask)
    {
        for (os_alarm_id_t i = 0U; i < BEERTOS_ALARM_ID_MAX; i++)
        {
            if (os_alarms[i].remaining_time > 0U && 0U != (os_alarm_active_mask & (1U << i)))
            {
                os_alarms[i].remaining_time--;

                if (0U == os_alarms[i].remaining_time)
                {
                    os_alarms[i].callback();

                    if (true == os_alarms[i].periodic)
                    {
                        os_alarms[i].remaining_time = os_alarms[i].period;
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

uint32_t os_alarm_get_remaining_time(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    return os_alarms[alarm_id].remaining_time;
}