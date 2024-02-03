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

void os_enter_critical_section(void)
{
    os_port_disable_interrupts();
    os_isr_counter++;
}

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

uint32_t os_get_tick_count(void)
{
    return os_tick_counter;
}
