
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

void timer_interrupt(void){
    // increment global system timer
    if(++systicks == 0) panic("systicks");
    // interrupt acknowledge
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_TIMER;
}

