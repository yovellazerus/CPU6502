
#include "comman.h"

static const char digits[] = "0123456789abcdef";
  
static void print_str(char* str){
    if(!str) str = "(null)";
    for (; *str; str++) uart_putc_sync(*str);
}

static void print_int(long number, int base, int sign){
    char buffer[20];
    bool negative;
    int i;

    negative = 0;
    if(sign && number < 0){
        negative = 1;
        number = -number;
    }

    i = 0;
    do{
        buffer[i++] = digits[number % base];
    } 
    while((number /= base) != 0);
    if(negative) buffer[i++] = '-';
    while(--i >= 0) uart_putc_sync(buffer[i]);
}

static void print_ptr(uint16_t ptr){
    int i;
    uart_putc_sync('0');
    uart_putc_sync('x');
    for (i = 0; i < (sizeof(uint16_t) * 2); i++, ptr <<= 4){
        uart_putc_sync(digits[ptr >> (sizeof(uint16_t) * 8 - 4)]);
    }
}

// main printk engine, only %d, %x, %p, %c, %s are in use
void vprintk(const char *fmt, va_list ap){
    int c0, c1, c2, i, state;

    state = 0;
    for(i = 0; fmt[i]; i++){

        c0 = fmt[i] & 0xff;

        if(state == 0){
            if(c0 == '%'){
                state = '%';
            } 
            else{
                uart_putc_sync(c0);
            }
        }
        else if(state == '%'){

            c1 = c2 = 0;
            if(c0) c1 = fmt[i + 1] & 0xff;
            if(c1) c2 = fmt[i + 2] & 0xff;

            if(c0 == 'd'){
                print_int(va_arg(ap, int), 10, 1);
            } 
            else if(c0 == 'l' && c1 == 'd'){
                print_int(va_arg(ap, long), 10, 1);
                i += 1;
            } 
            else if(c0 == 'u'){
                print_int(va_arg(ap, uint16_t), 10, 0);
            } 
            else if(c0 == 'l' && c1 == 'u'){
                print_int(va_arg(ap, uint32_t), 10, 0);
                i += 1;
            } 
            else if(c0 == 'x'){
                print_int(va_arg(ap, uint16_t), 16, 0);
            } 
            else if(c0 == 'l' && c1 == 'x'){
                print_int(va_arg(ap, uint32_t), 16, 0);
                i += 1;
            } 
            else if(c0 == 'p'){
                print_ptr(va_arg(ap, uint16_t));
            } 
            else if(c0 == 'c'){
                uart_putc_sync(va_arg(ap, uint16_t));
            } 
            else if(c0 == 's'){
                print_str(va_arg(ap, char *));
            } 
            else if(c0 == '%'){
                uart_putc_sync('%');
            } 
            else {
                // unknown format, print it for debug
                uart_putc_sync('%');
                uart_putc_sync(c0);
            }
            state = 0;
        }
    }
}

void printk(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintk(fmt, ap);
    va_end(ap);
}

void panic(const char *fmt, ...){
    va_list ap;
    __asm__("sei");
    va_start(ap, fmt);
    printk("PANIC: ");
    vprintk(fmt, ap);
    va_end(ap);
    while (true) { /* halt */ }
}
