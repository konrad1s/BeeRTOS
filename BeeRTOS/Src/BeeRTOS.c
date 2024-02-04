/******************************************************************************************
 * @brief OS main source file
 * @file BeeRTOS.c
 * ****************************************************************************************/

/******************************************************************************************
 *                                        INCLUDES                                        *
 ******************************************************************************************/

#include "BeeRTOS.h"
#include "BeeRTOS_internal.h"
#include "BeeRTOS_trace_cfg.h"

/******************************************************************************************
 *                                         DEFINES                                        *
 ******************************************************************************************/

#if (BEERTOS_ALARM_MODULE_EN == true)
#define OS_ALARM_INIT() os_alarm_module_init()
#else
#define OS_ALARM_INIT()
#endif

#if (BEERTOS_SEMAPHORE_MODULE_EN == true)
#define OS_SEMAPHORE_INIT() os_semaphore_module_init()
#else
#define OS_SEMAPHORE_INIT()
#endif

#if (BEERTOS_MUTEX_MODULE_EN == true)
#define OS_MUTEX_INIT() os_mutex_module_init()
#else
#define OS_MUTEX_INIT()
#endif

#if (BEERTOS_QUEUE_MODULE_EN == true)
#define OS_QUEUE_INIT() os_queue_module_init()
#else
#define OS_QUEUE_INIT()
#endif

#if (BEERTOS_MESSAGE_MODULE_EN == true)
#define OS_MESSAGE_INIT() os_message_module_init()
#else
#define OS_MESSAGE_INIT()
#endif

/******************************************************************************************
 *                                        TYPEDEFS                                        *
 ******************************************************************************************/

/******************************************************************************************
 *                                        VARIABLES                                       *
 ******************************************************************************************/

static volatile uint32_t os_isr_counter;
static volatile uint32_t os_tick_counter;

/******************************************************************************************
 *                                        FUNCTIONS                                       *
 ******************************************************************************************/
extern void os_cpu_init(void);
extern void os_port_disable_interrupts(void);
extern void os_port_enable_interrupts(void);

/**
 * @brief This function initializes the operating system. It initializes all the OS modules
 * and starts the OS scheduler (triggers the PendSV interrupt) used to switch context between
 * tasks. It should be called once by the application, before any other OS function!
 *
 * @param None
 * @return None
 */
void os_init(void)
{
    os_isr_counter = 0U;
    os_tick_counter = 0U;

    os_task_module_init();
    OS_ALARM_INIT();
    OS_SEMAPHORE_INIT();
    OS_MUTEX_INIT();
    OS_QUEUE_INIT();
    OS_MESSAGE_INIT();
    os_cpu_init();
    BEERTOS_TRACE_INIT();
}

/**
 * @brief This function enters critical section. It disables all interrupts. As many times
 * as os_enter_critical_section() is called, os_leave_critical_section() must be called to exit the critical
 * section.
 *
 * @param None
 * @return None
 */
void os_enter_critical_section(void)
{
    os_port_disable_interrupts();
    os_isr_counter++;
}

/**
 * @brief This function exits critical section. If the number of calls to os_enter_critical_section()
 * is equal to the number of calls to os_leave_critical_section(), the interrupts will be enabled.
 *
 * @param None
 * @return None
 */
void os_leave_critical_section(void)
{
    os_isr_counter--;
    if (os_isr_counter == 0U)
    {
        os_port_enable_interrupts();
    }
}

void os_tick(void)
{
    os_task_tick();
    os_alarm_tick();
    os_tick_counter++;
    BEERTOS_TRACE_TICK(os_tick_counter);
}

/**
 * @brief This function returns the current tick. The tick count is used to measure time in the OS.
 *
 * @param None
 * @return The current tick count (time in ticks)
 */
uint32_t os_get_tick_count(void)
{
    os_enter_critical_section();
    uint32_t tick_count = os_tick_counter;
    os_leave_critical_section();

    return tick_count;
}
