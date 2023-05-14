#include "ut_utils.h"

void ut_blocking_delay(uint32_t delay)
{
    uint32_t start = os_get_tick_count();
    
    while(os_get_tick_count() - start < delay)
    {
        __asm("nop");
    }
}