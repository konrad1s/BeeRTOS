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

/*! Structure to hold alarm data */
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

/*! List of all alarms */
static os_alarm_t os_alarms[BEERTOS_ALARM_ID_MAX];
/*! Mask of all active alarms */
static os_alarm_active_mask_t os_alarm_active_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
void os_alarm_init(void)
{
    os_alarm_active_mask = 0U;

    /*! X-Macro to initialize all alarms */
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

    os_alarm_t *const alarm = &os_alarms[alarm_id];

    alarm->period = period;
    alarm->periodic = periodic;
    alarm->remaining_time = period;

    OS_ALARM_ENABLE(alarm_id);
}

void os_alarm_cancel(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(os_alarm_active_mask & (1U << alarm_id), OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    os_alarms[alarm_id].remaining_time = 0U;
    OS_ALARM_DISABLE(alarm_id);
}

uint32_t os_alarm_get_remaining_time(os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX, OS_MODULE_ID_ALARM, OS_ERROR_INVALID_PARAM);

    return os_alarms[alarm_id].remaining_time;
}

void os_alarm_tick(void)
{
    /* Check if there are any active alarms */
    if (0U != os_alarm_active_mask)
    {
        /* Find all active alarms and decrement their remaining time */
        for (os_alarm_id_t alarm_id = 0U; alarm_id < BEERTOS_ALARM_ID_MAX; alarm_id++)
        {
            os_alarm_t *const alarm = &os_alarms[alarm_id];

            BEERTOS_ASSERT(alarm != NULL, OS_MODULE_ID_ALARM, OS_ERROR_NULLPTR);
            BEERTOS_ASSERT(alarm->callback != NULL, OS_MODULE_ID_ALARM, OS_ERROR_NULLPTR);

            if (alarm->remaining_time > 0U)
            {
                alarm->remaining_time--;

                if (0U == alarm->remaining_time)
                {
                    /* Remaining time expired, call the callback */
                    alarm->callback();

                    /* Rearm the alarm if it is periodic, otherwise disable it */
                    if (true == alarm->periodic)
                    {
                        alarm->remaining_time = alarm->period;
                    }
                    else
                    {
                        OS_ALARM_DISABLE(alarm_id);
                    }
                }
            }
        }
    }
}
