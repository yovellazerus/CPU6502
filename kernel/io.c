
#include "comman.h"

static const char digits[] = "0123456789abcdef";
  
static void putc(char c){
    while(!(MMIO8(UART_STAT) & UART_STATUS_TX_READY)){/* busy wait */};
    MMIO8(UART_TX) = c;
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
    while(--i >= 0) putc(buffer[i]);
}

static void print_ptr(uint16_t ptr){
    int i;
    putc('0');
    putc('x');
    for (i = 0; i < (sizeof(uint16_t) * 2); i++, ptr <<= 4)
        putc(digits[ptr >> (sizeof(uint16_t) * 8 - 4)]);
}

// main printf engine, only %d, %x, %p, %c, %s are in use
void vprintf(const char *fmt, va_list ap){
    char *s;
    int c0, c1, c2, i, state;

    state = 0;
    for(i = 0; fmt[i]; i++){

        c0 = fmt[i] & 0xff;

        if(state == 0){
            if(c0 == '%'){
                state = '%';
            } 
            else{
                putc(c0);
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
                putc(va_arg(ap, uint16_t));
            } 
            else if(c0 == 's'){
                if((s = va_arg(ap, char *)) == 0) s = "(null)";
                for (; *s; s++) putc(*s);
            } 
            else if(c0 == '%'){
                putc('%');
            } 
            else {
                // unknown format, print it for debug
                putc('%');
                putc(c0);
            }
            state = 0;
        }
    }
}

void printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void panic(const char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    printf("PANIC: ");
    vprintf(fmt, ap);
    va_end(ap);
}

static char getc(void)
{
    while(!(MMIO8(UART_STAT) & UART_STATUS_RX_READY)){/* busy wait */};
    return MMIO8(UART_RX);
}

uint16_t gets(char* buffer, int max)
{
    uint16_t i;
    char c;

    for (i = 0; i + 1 < max;){
        c = getc();
        putc(c); // TODO: remove echo
        buffer[i++] = c;
        if(c == '\n' || c == '\r' || c == '\0') break;
    }
    buffer[i] = '\0';
    return i;
}