
#include "comman.h"

// warp around using uint8_t overflow
struct Ring_Buffer {
    char    buffer[256];
    uint8_t head;
    uint8_t tail;
};

Ring_Buffer ring_buffer;

void uart_init(void){
    ring_buffer.head = ring_buffer.tail = 0;
}

// PUSH to ring buffer
void uart_rx_interrupt(void){

    uint8_t data;

    // for the sake of safety and for future development,
    // we interrogate the hardware to make sure there is real input in the UART RX register
    if (MMIO8(UART_STAT) & UART_STATUS_RX_READY) {
        
        data = MMIO8(UART_RX);

        if(ring_buffer.head + 1 != ring_buffer.tail){
            ring_buffer.buffer[ring_buffer.head] = data;
            ring_buffer.head++;
            wakeup(&ring_buffer);
        }
    }
    // else, ring buffer is full, trop the input

    // interrupt acknowledge
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_UART_RX;
}

// for printk, not using interrupts or yielding the CPU
void uart_putc_sync(char c){
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)){/* busy wait */};
    MMIO8(UART_TX) = c;
}

void uart_putc(char c){
    // yield the CPU until the tx register is empty
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)){
        proc_set_state(current_process, PROC_STATE_READY);
        scheduler();
    }
    MMIO8(UART_TX) = c;
}

// for gets in the kernel debugger, not using interrupts
int uart_getc_sync(void){
    while(!(MMIO8(UART_STAT) & UART_STATUS_RX_READY)){/* busy wait */};
    return MMIO8(UART_RX);
}

// POP from ring buffer
int uart_getc(void){
    uint16_t data;
    if(ring_buffer.head == ring_buffer.tail){
        // if ring buffer is empty
        return -1;
    }
    data = ring_buffer.buffer[ring_buffer.tail];
    ring_buffer.tail++;
    return data;
}
