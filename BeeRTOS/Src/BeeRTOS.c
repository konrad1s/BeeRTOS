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

#if (BEERTOS_ALARM_MODULE_EN == true)
    os_alarm_module_init();
#endif

#if (BEERTOS_SEMAPHORE_MODULE_EN == true)
    os_semaphore_module_init();
#endif

#if (BEERTOS_MUTEX_MODULE_EN == true)
    os_mutex_module_init();
#endif

#if (BEERTOS_QUEUE_MODULE_EN == true)
    os_queue_module_init();
#endif

#if (BEERTOS_MESSAGE_MODULE_EN == true)
    os_message_module_init();
#endif

    os_cpu_init();

    BEERTOS_TRACE_INIT();
}

void os_disable_all_interrupts(void)
{
    os_port_disable_interrupts();
    os_isr_counter++;
}

void os_enable_all_interrupts(void)
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
