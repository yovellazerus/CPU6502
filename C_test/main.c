#include <stdint.h>

#define MMIO_SCR_DATA 0xC00A
#define MMIO_SCR_CTRL 0xC00B

#define SCR_WRITE 1

volatile unsigned char *SCR_DATA = (unsigned char *)MMIO_SCR_DATA;
volatile unsigned char *SCR_CTRL = (unsigned char *)MMIO_SCR_CTRL;

void putchar(char c){
    *SCR_DATA = c;
    *SCR_CTRL = SCR_WRITE;
}

void puts(const char* str){
    while(*str){
        putchar(*str);
        str++;
    }
}

// A simple function that multiplies and divides
uint8_t compute(uint8_t a, uint8_t b) {
    uint8_t result = a * b;    // multiply
    result = result / 2;       // divide
    return result;
}

void main(void) {
    uint8_t x = 5;
    uint8_t y = 7;
    uint8_t z = compute(x, y); // function call with arithmetic

    volatile uint8_t* port = (uint8_t*)0x6000; // example memory-mapped IO
    *port = z;  // write result somewhere (like a register)
    puts("Hello C!\n");
    putchar('H');

    while (1);  // loop forever
}
