/******************************************************************************************
* @brief
* @file
* ****************************************************************************************/

/******************************************************************************************
*                                        INCLUDES                                        *
******************************************************************************************/

#include "BeeRTOS.h"
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

        /* Enable interrupts */
        "CPSIE          i                       \n"

        /* Return */
        " BX            lr                      \n"
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
    *(--stk) = 0x12121212;                /*!< R12 */
    *(--stk) = 0x03030303;                /*!< R3 */
    *(--stk) = 0x02020202;                /*!< R2 */
    *(--stk) = 0x01010101;                /*!< R1 */
    *(--stk) = (uint32_t)arg;             /*!< R0 */

    *(--stk) = 0x11111111;                /*!< R11 */
    *(--stk) = 0x10101010;                /*!< R10 */
    *(--stk) = 0x09090909;                /*!< R9 */
    *(--stk) = 0x08080808;                /*!< R8 */
    *(--stk) = 0x07070707;                /*!< R7 */
    *(--stk) = 0x06060606;                /*!< R6 */
    *(--stk) = 0x05050505;                /*!< R5 */
    *(--stk) = 0x04040404;                /*!< R4 */

    uint32_t* user_stack = stk;
    /* Fill the unused stack space with a known value */
    while (user_stack > (uint32_t *)stack_ptr)
    {
        *(--user_stack) = OS_TASK_STACK_PATTERN;
    }

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
    /* Trigger PendSV */
    PORT_NVIC_INT_CTRL |= PORT_NVIC_PENDSV_SET_MSK;
}

void SysTick_Handler(void)
{
    extern void os_tick(void);
    extern void os_sched(void);

    os_tick();
    os_sched();
}