#include "BeeRTOS.h"
#include "stm32f4xx.h"

#include "SEGGER_SYSVIEW.h"

static os_task *volatile os_task_current;
static os_task *volatile os_task_next;

#define OS_STACK_PATTERN 0xA5A5A5A5U

#define LOG2(x) (32 - __CLZ(x))

static os_task *os_tasks[OS_MAS_TASK_NB + 1];
uint32_t os_ready_mask;
uint32_t os_delay_mask;

uint32_t os_isr_cnt = 0U;

void os_init(void)
{
    uint32_t i;

    for (i = 0U; i < OS_MAS_TASK_NB; i++)
    {
        os_tasks[i] = NULL;
    }
    os_ready_mask = 0U;
    os_delay_mask = 0U;

    NVIC_SetPriority(PendSV_IRQn, 0xff);  /* Lowest possible priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00); /* Highest possible priority */
}

void os_run(void)
{
    os_sched();

    /* shall never be reached */
}

void os_disable_all_interrupts(void)
{
    __disable_irq();
    os_isr_cnt++;
}

void os_enable_all_interrupts(void)
{
    os_isr_cnt--;
    if (os_isr_cnt == 0U)
    {
        __enable_irq();
    }
}

void os_stack_monitor(void)
{

}

void os_stack_init(void)
{
    //uint32_t const * const curr_stack = __asm__ volatile("mov %0, sp" : "=r" (SP));
}

void os_delay(uint32_t ticks)
{
    os_disable_all_interrupts();
    os_task_current->ticks = ticks;
    os_ready_mask &= ~(1 << (os_task_current->priority - 1U));
    os_delay_mask |= (1 << (os_task_current->priority - 1U));
    os_sched();
    os_enable_all_interrupts();
}

void os_task_release(uint32_t task_id)
{
    os_disable_all_interrupts();
    os_ready_mask |= (1 << (task_id - 1U));
    os_delay_mask &= ~(1 << (task_id - 1U));
    os_sched();
    os_enable_all_interrupts();
}

void os_tick(void)
{
    uint32_t mask = os_delay_mask;
    uint32_t idx = 0;

    while (mask)
    {
       os_task *task = os_tasks[LOG2(mask)];

        task->ticks--;
         if (task->ticks == 0)
         {
              os_ready_mask |= (1 << (task->priority - 1U));
              os_delay_mask &= ~(1 << (task->priority - 1U));
         }
            mask &= ~(1 << (task->priority - 1U));
    }
    os_alarm_tick();
}

void os_sched(void)
{
    os_disable_all_interrupts();

    if (os_ready_mask == 0U)
    {
        os_task_next = os_tasks[0];
    }
    else
    {
        os_task_next = os_tasks[LOG2(os_ready_mask)];
    }

    if(os_task_current != os_task_next)
    {
        // SEGGER_SYSVIEW_OnTaskStopExec();
        os_task_next->priority ? SEGGER_SYSVIEW_OnTaskStartExec(os_task_next->priority) : SEGGER_SYSVIEW_OnIdle();
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
    os_enable_all_interrupts();
}

void os_task_delete(void)
{
    os_disable_all_interrupts();
    os_ready_mask &= ~(1 << (os_task_current->priority - 1U));
    os_delay_mask &= ~(1 << (os_task_current->priority - 1U));
    os_tasks[os_task_current->priority] = NULL;
    os_sched();
    os_enable_all_interrupts();
}

void os_task_create(os_task *task, os_task_handler task_handler,
                    void *stack, uint32_t stack_size, uint8_t priority)
{
    uint32_t *stack_ptr = (uint32_t *)((((uint32_t)stack + stack_size) / 8) * 8); //todo align
    uint32_t *stack_limit;

    *(--stack_ptr) = 0x01000000;               /*!< xPSR - THUMB bit set*/
    *(--stack_ptr) = (uint32_t)task_handler; /*!< PC */
    *(--stack_ptr) = 0x0000000EU;               /*!< LR */
    *(--stack_ptr) = 0x12121212;               /*!< R12 */
    *(--stack_ptr) = 0x03030303;               /*!< R3 */
    *(--stack_ptr) = 0x02020202;               /*!< R2 */
    *(--stack_ptr) = 0x01010101;               /*!< R1 */
    *(--stack_ptr) = 0x00000000;               /*!< R0 */
    *(--stack_ptr) = 0x11111111;               /*!< R11 */
    *(--stack_ptr) = 0x10101010;               /*!< R10 */
    *(--stack_ptr) = 0x09090909;               /*!< R9 */
    *(--stack_ptr) = 0x08080808;               /*!< R8 */
    *(--stack_ptr) = 0x07070707;               /*!< R7 */
    *(--stack_ptr) = 0x06060606;               /*!< R6 */
    *(--stack_ptr) = 0x05050505;               /*!< R5 */
    *(--stack_ptr) = 0x04040404;               /*!< R4 */

    /* Set stack pointer */
    task->sp = (void *)stack_ptr;
    /* Round up the bottom of the stack to the 8-byte boundary */
    stack_limit = (uint32_t *)(((((uint32_t)stack - 1U) / 8) + 1U) * 8);

    for (stack_ptr = stack_ptr - 1U; stack_ptr > stack_limit; stack_ptr--)
    {
        *stack_ptr = 0xDEADBEEF;
    }

    if(priority > 0U)
    {
        os_ready_mask |= (1 << (priority - 1U));
    }
    task->priority = priority;
    task->ticks = 0U;

    if(priority < OS_MAS_TASK_NB)
    {
        os_tasks[priority] = task; //TODO: assert
    }
}

os_task* os_get_current_task(void)
{
    return os_task_current;
}

__attribute__ ((naked, optimize("-fno-stack-protector")))
void PendSV_Handler(void)
{
__asm volatile
(
    // __ASM volatile ("cpsid i" : : : "memory");
    "CPSID          i \n" /* Disable interrupts */

    // if(os_task_current != (os_task *) 0)
    "LDR            R3, =os_task_current \n"
    "LDR            R3, [R3] \n"
    "CMP            R3, #0 \n"
    "BEQ            PendSV_restore \n"

    /* push registers */
    "PUSH           {R4-R11} \n"
    
    "LDR            R3, =os_task_current \n"
    "LDR            R3, [R3] \n"
    // os_task_current->sp = sp;
    "STR            sp, [R3] \n"
    
"PendSV_restore: \n"
    // sp = os_task_next->sp;
    "LDR            R3, =os_task_next \n"
    "LDR            R3, [R3] \n"
    "LDR            sp, [R3] \n"

    // os_task_current = os_task_next;
    "LDR            R3, =os_task_next \n"
    "LDR            R3, [R3] \n"
    "LDR            R2, =os_task_current \n"
    "STR            R3, [R2] \n"

    // pop registers
    "POP            {r4-r11} \n"

    // __ASM volatile ("cpsie i" : : : "memory");
    "CPSIE          i \n"

    /* return to the next task */
    " BX            lr                \n"
);
}

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  //SEGGER_SYSVIEW_RecordEnterISR();
  os_tick();
  os_sched();
  //SEGGER_SYSVIEW_RecordExitISR();
  /* USER CODE END SysTick_IRQn 0 */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}