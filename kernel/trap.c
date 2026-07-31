
#include "comman.h"

/* 
the kernel syscall entry.
*/
void kernel_brk(void){
    uint8_t sys_number;
    Syscall syscall;
    uint16_t return_value = -1;

    kernel_prologue();

    // get the syscall and call it
    sys_number = proc_get_ctx(current_process)->y;
    syscall = syscalls_table[sys_number];
    if(!syscall){
        printk("kernel: \"%s\" [%d] terminated do to invalid syscall number: 0x%x\n", proc_get_name(current_process), proc_get_pid(current_process), sys_number);
        proc_set_ax(current_process, BADSYSCALL);
        sys_exit();
    }
    else{
        // NOTE: it is the responsibility of the system call to fetch and parse it's arguments
        return_value = syscall();
    }

    proc_set_ax(current_process, return_value);

    kernel_epilogue();
    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

/*
kernel NMI handler, it serves as a "watchdog" for security purposes.
it's job is to terminate a user process that try to execute an invalid opcode.
and in addition to make shore user processes cant set "I" flag on, and keep kernel space safe,
by prohibit user code form execute "SEI"/"PLP"/"RTI" instructions.
the "watchdog" is the only source for NMI's in this system
*/
void kernel_nmi(void){

    const char* mnemonic;
    uint8_t watchdog = MMIO8(MMU_WATCHDOG_REGISTER);

    kernel_prologue();

    switch (watchdog)
    {
        case 0x78:
            mnemonic = "SEI";
            break;
        case 0x28:
            mnemonic = "PLP";
            break;
        case 0x40:
            mnemonic = "RTI";
            break;
        
        default:
            printk("kernel: prosess [%d] \"%s\" was terminated do to executing invalid opcode: <0x%x>\n", 
                proc_get_pid(current_process), proc_get_name(current_process), watchdog);
            goto end;
    }
    printk("kernel: prosess [%d] \"%s\" was terminated do to executing \"%s\" instruction\n", 
            proc_get_pid(current_process), proc_get_name(current_process), mnemonic);
end:
    proc_set_ax(current_process, WATCHDOG);
    sys_exit();   
    // no return 
}

/*
this function handles hardware interrupts from system devices,
it differs from device_interrupt() in that it is called from user space, 
whereas device_interrupt() can also be called from an IRQ in kernel space.
*/
void kernel_irq(void){

    if(!current_process){
        panic("is this possible?");
    }

    kernel_prologue();

    // if true it is the timer
    if(device_interrupt()){
        /* 
        the kernel timer interrupt handler, 
        if the process has any quantum remaining, it will return to user space here.
        otherwise, it will by pass to the scheduler() and yield the CPU.
        */
        if(proc_ticks_dec(current_process) == 0){

            proc_set_state(current_process, PROC_STATE_READY);

            scheduler();
        }
    }
    // else, not the timer

    // process has quantum remaining, so we will return to it
    kernel_epilogue();

    // return to irq_handler in trampoline.s that called it, 
    // and the trampoline will jump to: return_from_trap and resume the user processs
}

// this is doing the work of a device IRQ
// TODO: only Timer IRQ's for now
bool device_interrupt(void){

    // increment global system timer
    if(++systicks == 0) panic("systicks");

    return true; // for timer interrupt
}

/*
Interactive kernel debugger,
it is called from "BRK" in kernel space
*/
void kernel_debugger(void) {
    char input[32];
    Context ctx;
    get_cpu_state(&ctx);
    printk("********** kernel break point ***********");
    print_cpu_state(&ctx);
    print_process_state(current_process);

    printk("Type 'h' or 'help' for available debugger commands.\n\n");

    while (1) {
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
