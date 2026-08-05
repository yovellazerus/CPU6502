
#include "comman.h"

volatile uint32_t systicks = 0; 
volatile uint32_t next_wakeup_call = 0xffffffff; // maximum value, so the timer ignores it by default

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

    // interrupt acknowledge
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_TIMER;

    // increment global system timer
    if(++systicks == 0) panic("systicks");

    // only call the heavy wakeup() function if a process timer has expired
    // next_wakeup_call is updated by a process calling the sys_sleep syscall
    if(systicks >= next_wakeup_call){
        wakeup((void*)&systicks);
        // maximum value, so the timer ignores it by default
        next_wakeup_call = 0xFFFFFFFF;
    }
}

