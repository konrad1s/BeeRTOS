/******************************************************************************************
 * @brief OS alarm source file
 * @file BeeRTOS_alarm.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS_alarm.h"
#include "BeeRTOS_assert.h"
#include "BeeRTOS_trace_cfg.h"
#include "BeeRTOS_task.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#define OS_ALARM_SET_MASK(mask, alarm_id) (mask |= (1ULL << alarm_id))
#define OS_ALARM_CLEAR_MASK(mask, alarm_id) (mask &= ~(1ULL << alarm_id))

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

#undef BEERTOS_ALARM
#define BEERTOS_ALARM(...) +1U
/*! Returns the number of alarms, BEERTOS_ALARM_ID_MAX cannot be used in preprocessor expressions,
    because enum is known only after the preprocessor is done */
#define OS_ALARM_COUNT (0U + BEERTOS_ALARM_LIST())

/*! Check and select the proper mask type for the number of configured alarms */
#if (OS_ALARM_COUNT <= 8U)
    typedef uint8_t os_alarm_active_mask_t;
#elif (OS_ALARM_COUNT <= 16U)
    typedef uint16_t os_alarm_active_mask_t;
#elif (OS_ALARM_COUNT <= 32U)
    typedef uint32_t os_alarm_active_mask_t;
#elif (OS_ALARM_COUNT <= 64U)
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
static os_alarm_active_mask_t os_alarm_pending_mask;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
static void os_alarm_init(os_alarm_t *const alarm,
                          const uint32_t period,
                          const bool periodic,
                          void (*callback)(void))
{
    BEERTOS_ASSERT(callback != NULL, OS_MODULE_ID_ALARM, OS_ERROR_NULLPTR);

    alarm->period = period;
    alarm->periodic = periodic;
    alarm->callback = callback;
    alarm->remaining_time = 0U;
}

/**
 * @brief The function initializes the operating system's alarms.
 * It is responsible for setting up any necessary data structures.
 * Called once (automatically) in os system initialization.
 * It is also responsible for starting the alarms that are set to autostart.
 *
 * @param None
 * @return None
 */
void os_alarm_module_init(void)
{
    os_alarm_active_mask = 0U;
    os_alarm_pending_mask = 0U;

    /*! X-Macro to initialize all alarms */
    #undef BEERTOS_ALARM
    #define BEERTOS_ALARM(name, _callback, _autostart, _period, _periodic)  \
        os_alarm_init(&os_alarms[name], _period, _periodic, _callback);     \
        if (true == _autostart)                                             \
        {                                                                   \
            os_alarm_start(name, _period, _periodic);                       \
        }
    
    #define OS_ALARM_INIT_ALL() BEERTOS_ALARM_LIST()
    OS_ALARM_INIT_ALL();
}

/**
 * @brief This function starts the specified alarm. If the alarm is already started, the function
 * will restart it. If the alarm is already started and periodic, the function will restart it
 * with the new period.
 * 
 * @param alarm_id - alarm id
 * @param period - alarm period in ticks
 * @param periodic - true if the alarm is periodic, false if it is one-shot
 * @return None
 */
void os_alarm_start(const os_alarm_id_t alarm_id, const uint32_t period, const bool periodic)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX,
                   OS_MODULE_ID_ALARM,
                   OS_ERROR_INVALID_PARAM);

    os_alarm_t *const alarm = &os_alarms[alarm_id];

    os_enter_critical_section();
    alarm->period = period;
    alarm->periodic = periodic;
    alarm->remaining_time = period;

    OS_ALARM_SET_MASK(os_alarm_active_mask, alarm_id);
    os_leave_critical_section();

    BEERTOS_TRACE_ALARM_START(alarm_id, period, periodic);
}

/**
 * @brief This function cancels the specified alarm. The alarm has to be started before it can be
 * cancelled.
 * 
 * @param alarm_id - alarm id
 * @return None
 */
void os_alarm_cancel(const os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX,
                   OS_MODULE_ID_ALARM,
                   OS_ERROR_INVALID_PARAM);
    BEERTOS_ASSERT(os_alarm_active_mask & (1U << alarm_id),
                   OS_MODULE_ID_ALARM,
                   OS_ERROR_INVALID_PARAM);

    os_enter_critical_section();
    os_alarms[alarm_id].remaining_time = 0U;
    OS_ALARM_CLEAR_MASK(os_alarm_active_mask, alarm_id);
    os_leave_critical_section();

    BEERTOS_TRACE_ALARM_CANCEL(alarm_id);
}

/**
 * @brief This function returns the remaining time until the alarm expires.
 * If the alarm is not started, the function will return 0.
 * 
 * @param alarm_id - alarm id
 * @return remaining time in ticks
 */
uint32_t os_alarm_get_remaining_time(const os_alarm_id_t alarm_id)
{
    BEERTOS_ASSERT(alarm_id < BEERTOS_ALARM_ID_MAX,
                   OS_MODULE_ID_ALARM,
                   OS_ERROR_INVALID_PARAM);

    os_enter_critical_section();
    const uint32_t remaining_time = os_alarms[alarm_id].remaining_time;
    os_leave_critical_section();

    return remaining_time;
}

/**
 * @brief This function is called by the OS tick handler. It is responsible for decrementing the
 * remaining time of the active alarms and calling the alarm callbacks when the remaining time
 * reaches 0.
 * 
 * @param None
 * @return None
 */
void os_alarm_tick(void)
{
    os_enter_critical_section();

    /* Check if there are any active alarms */
    if (0U != os_alarm_active_mask)
    {
        /* Find all active alarms and decrement their remaining time */
        for (os_alarm_id_t id = 0U; id < BEERTOS_ALARM_ID_MAX; id++)
        {
            if (os_alarm_active_mask & (1ULL << id))
            {
                os_alarm_t *const alarm = &os_alarms[id];

                if (alarm->remaining_time > 0U)
                {
                    alarm->remaining_time--;

                    if (0U == alarm->remaining_time)
                    {
                        /* Set the alarm as pending, callback will be called in the alarm task */
                        OS_ALARM_SET_MASK(os_alarm_pending_mask, id);
                    }
                }
            }
        }

        /* If there are any pending alarms, release the alarm task */
        if (os_alarm_pending_mask)
        {
            os_task_release(OS_ALARM_TASK);
        }
    }

    os_leave_critical_section();
}

/**
 * @brief This function is the task that is responsible for calling the alarm callbacks.
 * 
 * @param arg - not used
 * @return None
 */
void os_alarm_task(void *const arg)
{
    (void)arg;

    /* While 1, because the task is stopped after processing the alarms */
    while (1)
    {
        os_enter_critical_section();
        os_alarm_active_mask_t mask = os_alarm_pending_mask;
        os_leave_critical_section();

        /* Process all pending alarms */
        while (mask)
        {
            const os_alarm_id_t id = OS_GET_HIGHEST_PRIO_TASK_FROM_MASK(mask) - 1U;
            os_alarm_t *const alarm = &os_alarms[id];
            BEERTOS_ASSERT(alarm->callback != NULL,
                           OS_MODULE_ID_ALARM,
                           OS_ERROR_NULLPTR);

            alarm->callback();

            os_enter_critical_section();
            /* Rearm the alarm if it is periodic, otherwise disable it */
            if (alarm->periodic)
            {
                alarm->remaining_time = alarm->period;
                OS_ALARM_SET_MASK(os_alarm_active_mask, id);
            }
            else
            {
                OS_ALARM_CLEAR_MASK(os_alarm_active_mask, id);
            }

            OS_ALARM_CLEAR_MASK(os_alarm_pending_mask, id);
            os_leave_critical_section();

            /* Update the mask to exclude the just processed alarm */
            OS_ALARM_CLEAR_MASK(mask, id);
        }

        /* Stop the task after processing all alarms */
        os_task_stop(OS_ALARM_TASK);
    }
}
