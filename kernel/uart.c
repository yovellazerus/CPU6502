
#include "comman.h"

// warp around using uint8_t overflow
struct Ring_Buffer {
    uint8_t buffer;
    uint8_t head;
    uint8_t tail;
};

void Ring_Buffer_push(Ring_Buffer* rb, char c){

}

char Ring_Buffer_pop(Ring_Buffer* rb){
    
}

void uart_rx_interrupt(void){

    // TODO: push to ring buffer, or sleep

    // interrupt acknowledge
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_UART_RX;
}

// for printk, not using interrupts
void uart_putc_sync(char c){
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)){/* busy wait */};
    MMIO8(UART_TX) = c;
}

// for gets and kernel debugger, not using interrupts
char uart_getc_sync(void){
    while(!(MMIO8(UART_STAT) & UART_STATUS_RX_READY)){/* busy wait */};
    return MMIO8(UART_RX);
}

char uart_getc(void){

}
