
#include "comman.h"

volatile uint32_t systicks = 0; 

// configures and starts the periodic hardware IRQ timer
void timer_init(void){
    MMIO16(TIMER_LATCH_LOW) = CYCLES;
    MMIO8(TIMER_CTRL) = TIMER_ENABLE_TRUE;
}

void timer_resume(void){
    MMIO8(TIMER_CTRL) = TIMER_ENABLE_TRUE;
}

void timer_pause(void){
    MMIO8(TIMER_CTRL) = TIMER_ENABLE_FALSE;
}

