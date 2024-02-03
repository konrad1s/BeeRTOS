/******************************************************************************************
* @brief
* @file
* ****************************************************************************************/

/******************************************************************************************
*                                        INCLUDES                                        *
******************************************************************************************/

#include "BeeRTOS.h"
#include "BeeRTOS_trace_cfg.h"
#include "os_portable.h"

/******************************************************************************************
*                                         DEFINES                                        *
******************************************************************************************/
/* Register addresses */
#define PORT_NVIC_SYSTICK_CTRL              (*((volatile uint32_t *)0xE000E010U))
#define PORT_NVIC_SYSTICK_LOAD              (*((volatile uint32_t *)0xE000E014U))
#define PORT_NVIC_SYSTICK_VAL               (*((volatile uint32_t *)0xE000E018U))
#define PORT_NVIC_SYSPRI2                   (*((volatile uint32_t *)0xE000ED20U))

#define PORT_NVIC_INT_CTRL                  (*((volatile uint32_t *)0xE000ED04U))
#define PORT_NVIC_PENDSV_SET_MSK            (1UL << 28U)

/* Interrupt priority mask */
#define PORT_LOWEST_INTERRUPT_PRIORITY      (0xFFU)
#define PORT_HIGHEST_INTERRUPT_PRIORITY     (0x00U)

#define PORT_NVIC_PENDSV_PRI                (PORT_LOWEST_INTERRUPT_PRIORITY << 16U)
#define PORT_NVIC_SYSTICK_PRI               (PORT_HIGHEST_INTERRUPT_PRIORITY << 24U)

/******************************************************************************************
*                                        TYPEDEFS                                        *
******************************************************************************************/

/******************************************************************************************
*                                        VARIABLES                                       *
******************************************************************************************/

extern os_task_t *volatile os_task_current;
extern os_task_t *volatile os_task_next;

/******************************************************************************************
*                                        FUNCTIONS                                       *
******************************************************************************************/
void os_context_switched_cb(void)
{
    BEERTOS_TRACE_TASK_SWITCHED(os_task_next);
}

__attribute__ ((naked, optimize("-fno-stack-protector")))
void PendSV_Handler(void)
{
    __asm volatile
    (
        /* Disable interrupts */
        "CPSID          i                       \n"

        /* Check if os_task_current is NULL */
        "LDR            R3, =os_task_current    \n"
        "LDR            R3, [R3]                \n"
        "CMP            R3, #0                  \n"
        "BEQ            PendSV_restore          \n"

        /* Save registers */
        "PUSH           {R4-R11}                \n"

        /* Save the stack pointer */
        "LDR            R3, =os_task_current    \n"
        "LDR            R3, [R3]                \n"
        "STR            sp, [R3]                \n"

        "PendSV_restore:                            \n"
        /* Restore the stack pointer */
        "LDR            R3, =os_task_next       \n"
        "LDR            R3, [R3]                \n"
        "LDR            sp, [R3]                \n"

        /* Load the next task */
        "LDR            R3, =os_task_next       \n"
        "LDR            R3, [R3]                \n"
        "LDR            R2, =os_task_current    \n"
        "STR            R3, [R2]                \n"

        /* Restore registers */
        "POP            {r4-r11}                \n"

    #ifdef BEERTOS_TRACE_TASK_SWITCHED
        "stmdb          sp!, {r4-r11, lr}       \n"
        "bl             os_context_switched_cb  \n"
        "ldmia          sp!, {r4-r11, lr}       \n"
    #endif /* BEERTOS_TRACE_TASK_SWITCHED */

        /* Enable interrupts */
        "CPSIE          i                       \n"

        /* Return */
        " BX            r14                      \n"
        );
}

void os_port_disable_interrupts(void)
{
    __asm volatile
    (
        "cpsid i" : : : "memory"
    );
}

void os_port_enable_interrupts(void)
{
    __asm volatile
    (
        "cpsie i" : : : "memory"
    );
}

os_stack_t* os_port_task_stack_init(void (*task)(void *), void *arg, void *stack_ptr, uint32_t stack_size)
{
    /* Align the stack to 8 bytes */
    uint32_t *stk = (uint32_t *)(((uint32_t)stack_ptr + stack_size) & ~0x7U);

    /* Initialize the stack frame */
    *(--stk) = 0x01000000;                /*!< xPSR - THUMB bit set*/
    *(--stk) = (uint32_t)task;            /*!< PC */
    *(--stk) = 0x0000000EU;               /*!< LR */

    stk -= 4U;                            /*!< R12, R3-R1 */

    *(--stk) = (uint32_t)arg;             /*!< R0 */

    stk -= 8U;                            /*!< R11-R4 */

    #if (BEERTOS_USE_TASK_STACK_MONITOR == true)
        uint32_t* user_stack = stk;
        /* Fill the unused stack space with a known value */
        while (user_stack > (uint32_t *)stack_ptr)
        {
            *(--user_stack) = OS_TASK_STACK_PATTERN;
        }
    #endif

    return stk;
}

void os_cpu_init(void)
{
    /* PendSV_IRQn lowest possible priority */
    PORT_NVIC_SYSPRI2 |= PORT_NVIC_PENDSV_PRI;
    /* SysTick_IRQn highest possible priority */
    PORT_NVIC_SYSPRI2 |= PORT_NVIC_SYSTICK_PRI;
}

void os_port_context_switch(void)
{
    BEERTOS_TRACE_EXIT_ISR_SCHEDULER();
    /* Trigger PendSV */
    PORT_NVIC_INT_CTRL |= PORT_NVIC_PENDSV_SET_MSK;
}

void SysTick_Handler(void)
{
    BEERTOS_TRACE_ENTER_ISR();

    extern void os_tick(void);
    extern void os_sched(void);

    os_tick();
    os_sched();

    BEERTOS_TRACE_EXIT_ISR();
}