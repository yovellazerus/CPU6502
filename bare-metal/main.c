// main.c — simple bare-metal C program for 6502

#define MMIO_SCR_DATA 0xC00A
#define MMIO_SCR_CTRL 0xC00B

#define SCR_WRITE 1

volatile unsigned char *SCR_DATA = (unsigned char *)MMIO_SCR_DATA;
volatile unsigned char *SCR_CTRL = (unsigned char *)MMIO_SCR_CTRL;

void putchar(char c){
    *SCR_DATA = c;
    *SCR_CTRL = SCR_WRITE;
}

void hello(void){

    putchar('H');
    putchar('e');
    putchar('l');
    putchar('l');
    putchar('o');
    putchar(' ');
    putchar('W');
    putchar('o');
    putchar('r');
    putchar('l');
    putchar('d');
    putchar('!');
    putchar('\n');
}

char* msg = "Hello World!\n";

void main(void) {

    hello();
    
    for (;;) {
        /* infinite loop */
    }
}
