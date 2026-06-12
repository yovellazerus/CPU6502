
#include <stdint.h>

#define UART_TX 0xfe10

#define MMIO(register) *(volatile uint8_t*)(register)

void putchar(char c){
    while(MMIO(UART_TX)) {/* busy wait */};
    MMIO(UART_TX) = c;
}

void puts(const char* str){
    while(*str){
        putchar(*str);
        str++;
    }
}

void main(void) {
    
    puts("Hello C!\n");

    while (1);  // loop forever
}
