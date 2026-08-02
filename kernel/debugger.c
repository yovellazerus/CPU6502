
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
    printk("\n********** kernel break point ***********\n\n");
    print_cpu_state(&ctx);
    print_process_state(current_process);

    printk("\nType 'h' for available debugger commands.\n");

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
                    "  r - Display CPU registers & flags\n"
                    "  p - Display current process details\n"
                    "  c - Continue execution (skips the BRK instruction)\n"
                    "  k - Force a kernel panic and crash log\n"
                );
        }
        else if (strcmp(input, "r") == 0) {
            print_cpu_state(&ctx);
        }
        else if (strcmp(input, "p") == 0) {
            print_process_state(current_process);
        }
        else if (strcmp(input, "c") == 0) {
            printk("Continuing execution...\n");
            break; 
        }
        else if (strcmp(input, "k") == 0) {
            panic("manual kernel panic requested via debugger");
        }
        else if (strlen(input) > 0) {
            printk("Unknown command '%s'. Type 'h' for a list of commands.\n", input);
        }
    }
}

void print_process_state(Proc* p) {
    uint8_t i;
    printk(" Process Control Block:\n\n");
    if (p != NULL) {
        printk( "   Name : \"%s\"\n" 
                "   PID  : %d\n" 
                "   Page Table: ",
            proc_get_name(p), proc_get_pid(p));
        for (i = 0; i < PAGE_TABLE_SIZE; i++) {
            if(i % 4 == 0) printk("\n\t");
            printk("0x%x ", proc_get_page_table(p)[i]);
        }
    } else {
        printk("\n    (null)\n");
    }
    printk("\n");
}

void print_cpu_state(Context* ctx) {
    printk( " CPU state and registers:\n\n"
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
