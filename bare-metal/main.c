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

void putstring(char* str){
    while(*str){
        putchar(*str);
        str++;
    }
}

void hello(void){
    *SCR_DATA = 'H';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'e';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'l';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'l';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'o';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = ' ';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'W';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'o';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'r';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'l';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = 'd';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = '!';
    *SCR_CTRL = SCR_WRITE;

    *SCR_DATA = '\n';
    *SCR_CTRL = SCR_WRITE;
}

char* msg = "Hello World!\n";

void main(void) {

    putstring(msg);
    putchar('X');
    hello();
    
    for (;;) {
        /* infinite loop */
    }
}
