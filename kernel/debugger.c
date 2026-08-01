
#include "comman.h"

/*
Interactive kernel debugger,
it is called from "BRK" in kernel space
NOTE: not from user space! can not be used to debug user code and user context
TODO: this "eats" to much RAM... consider remove it...
*/
void kernel_debugger(void) {
    char input[32];
    Context ctx;
    get_cpu_state(&ctx);
    printk("********** kernel break point ***********");
    print_cpu_state(&ctx);
    print_process_state(current_process);

    printk("Type 'h' or 'help' for available debugger commands.\n\n");

    while (true) {
        printk("> ");
        
        // clear buffer and read input
        memset(input, 0, sizeof(input));
        if (gets(input, sizeof(input)) == 0) {
            continue;
        }

        // trim trailing newline if present
        *strchr(input, '\r') = 0;

        if (strcmp(input, "h") == 0 || strcmp(input, "help") == 0) {
            printk("Available Commands:\n");
            printk("  r, regs   - Display CPU registers & flags\n");
            printk("  p, proc   - Display current process details\n");
            printk("  c, cont   - Continue execution (skips the BRK instruction)\n");
            printk("  k, panic  - Force a kernel panic and crash log\n");
        }
        else if (strcmp(input, "r") == 0 || strcmp(input, "regs") == 0) {
            print_cpu_state(&ctx);
        }
        else if (strcmp(input, "p") == 0 || strcmp(input, "proc") == 0) {
            print_process_state(current_process);
        }
        else if (strcmp(input, "c") == 0 || strcmp(input, "cont") == 0) {
            printk("Continuing execution...\n");
            break; 
        }
        else if (strcmp(input, "k") == 0 || strcmp(input, "panic") == 0) {
            panic("manual kernel panic requested via debugger");
        }
        else if (strlen(input) > 0) {
            printk("Unknown command '%s'. Type 'help' for a list of commands.\n", input);
        }
    }
}

void print_process_state(Proc* p) {
    uint8_t i;
    printk(" Process Control Block:\n");
    if (p != NULL) {
        printk("   Name : \"%s\"\n", proc_get_name(p));
        printk("   PID  : %d\n", proc_get_pid(p));
        printk("   Page Table: ");
        for (i = 0; i < PAGE_TABLE_SIZE; i++) {
            if(i % 4 == 0) printk("\n\t");
            printk("0x%x ", proc_get_page_table(p)[i]);
        }
    } else {
        printk("\n    (null)\n");
    }
    printk("\n+---------------------------------------+\n");
}

void print_cpu_state(Context* ctx) {
    printk("\n+---------------------------------------+\n");
    printk("|         CPU state and registers       |\n");
    printk("+---------------------------------------+\n");
    printk(" \tPC = %p       SP = 0x01%x\n", (unsigned int)ctx->pc, ctx->sp);
    printk("   A  = 0x%x     X  = 0x%x      Y  = 0x%x\n", ctx->a, ctx->x, ctx->y);
    printk("   P  = 0x%x    [ %c %c - %c %c %c %c %c ]\n", 
        ctx->p,
        (ctx->p & FLAG_N) ? 'N' : '-',
        (ctx->p & FLAG_V) ? 'V' : '-',
        (ctx->p & FLAG_B) ? 'B' : '-',
        (ctx->p & FLAG_D) ? 'D' : '-',
        (ctx->p & FLAG_I) ? 'I' : '-',
        (ctx->p & FLAG_Z) ? 'Z' : '-',
        (ctx->p & FLAG_C) ? 'C' : '-'
    );
    printk("+---------------------------------------+\n");
}
