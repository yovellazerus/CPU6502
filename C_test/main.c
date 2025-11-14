#include <stdint.h>

#define MMIO_SCR_DATA 0xC00A
#define MMIO_SCR_CTRL 0xC00B

#define SCR_WRITE 1

void putchar(char c){
    *(volatile uint8_t*)MMIO_SCR_DATA = c;
    *(volatile uint8_t*)MMIO_SCR_CTRL = SCR_WRITE;
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
