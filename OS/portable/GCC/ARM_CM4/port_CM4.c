#include "BeeRTOS.h"

extern os_task *volatile os_task_current;
extern os_task *volatile os_task_next;

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