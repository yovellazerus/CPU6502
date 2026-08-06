
#include "comman.h"

/*
Interactive kernel debugger,
it is called from "BRK" in kernel space
NOTE: not from user space! can not be used to debug user code and user context
*/
void kernel_debugger(void) {
    char input[32];
    Context ctx;
    get_cpu_state(&ctx);
    printk("\nKERNEL BREAK POINT:\n\n");
    print_cpu_state(&ctx);
    print_process_state(current_process);

    printk("\nType 'h' for help\n");

    while (true) {
        printk("> ");
        
        // clear buffer and read input
        memset(input, 0, sizeof(input));
        if (gets(input, sizeof(input)) == 0) {
            continue;
        }

        // trim trailing newline if present
        *strchr(input, '\r') = 0;

        if (strcmp(input, "h") == 0) {
            printk( "Available Commands:\n"
                    "  r - Registers\n"
                    "  p - Process\n"
                    "  c - Continue\n"
                    "  h - Help\n"
                );
        }
        else if (strcmp(input, "r") == 0) {
            print_cpu_state(&ctx);
        }
        else if (strcmp(input, "p") == 0) {
            print_process_state(current_process);
        }
        else if (strcmp(input, "c") == 0) {
            printk("continuing...\n");
            break; 
        }
        else if (strlen(input) > 0) {
            printk("Unknown command '%s'.\n", input);
        }
    }
}

void print_process_state(Proc* p) {
    uint8_t i;
    printk(" Process:\n\n");
    if (p != NULL) {
        printk( "\tName : \"%s\"\n" 
                "\tPID  : %d\n" 
                "\tPage Table: ",
            proc_get_name(p), proc_get_pid(p));
        for (i = 0; i < PAGE_TABLE_SIZE; i++) {
            if(i % 4 == 0) printk("\n\t");
            printk("0x%x ", proc_get_page_table(p)[i]);
        }
        printk("\n\tKernel Mem:\n\t");
        for (i = 0; i < 3; i++) {
            printk("0x%x ", proc_get_kernel_low_memory(p)[i]);
        }
    } else {
        printk("\n\t(null)\n");
    }
    printk("\n");
}

void print_cpu_state(Context* ctx) {
    printk( " CPU registers:\n\n"
            "   PC = %p     SP = 0x01%x\n"
            "   A = 0x%x,  X = 0x%x,  Y = 0x%x\n"
            "   P = 0x%x [ %c %c - %c %c %c %c %c ]\n", 

        (unsigned int)ctx->pc, ctx->sp,
        ctx->a, ctx->x, ctx->y,
        ctx->p,
        (ctx->p & FLAG_N) ? 'N' : '-',
        (ctx->p & FLAG_V) ? 'V' : '-',
        (ctx->p & FLAG_B) ? 'B' : '-',
        (ctx->p & FLAG_D) ? 'D' : '-',
        (ctx->p & FLAG_I) ? 'I' : '-',
        (ctx->p & FLAG_Z) ? 'Z' : '-',
        (ctx->p & FLAG_C) ? 'C' : '-'
    );
    printk("\n");
}

uint16_t gets(char* buffer, int max)
{
    uint16_t i;
    char c;

    for (i = 0; i + 1 < max;){
        c = uart_getc_sync();
        uart_putc_sync(c);
        buffer[i++] = c;
        if(c == '\n' || c == '\r' || c == '\0') break;
    }
    buffer[i] = '\0';
    return i;
}
