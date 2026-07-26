
#include "comman.h"

volatile uint32_t systicks = 0; 

// doesn't start the timer, it only configures it
void timer_init(void){
    MMIO16(TIMER_LATCH_LOW) = CIRCLES;
    MMIO16(TIMER_COUNTER_LOW) = CIRCLES;
    MMIO8(TIMER_DISABLE) = TIME_ENABLE_FALSE;
}

void timer_resume(void){
    MMIO8(TIMER_ENABLE) = TIME_ENABLE_TRUE;
}

void timer_puse(void){
    MMIO8(TIMER_DISABLE) = TIME_ENABLE_FALSE;
}

