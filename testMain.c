
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

#include <conio.h>

static bool is_dubbing = false;

// very basic input system for now and not portable....
typedef enum {
    NO_INT = 0,
    NMI,
    RESET,
    IRQ,
} interrupt;

interrupt trigger_interrupt(){
    if (_kbhit()) {
        char c = _getch();
        if(c == 'n'){
            return NMI;
        }
        else if(c == 'r'){
            return RESET;
        }
        else if(c == 'i'){
            return IRQ;
        }
    }
    return NO_INT;
}

void basic_test(CPU* cpu, Memory* memory, FILE* cpu_file, FILE* memory_file, FILE* stack_file);

int main(int argc, char* argv[])
{

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-dubbing") == 0){
            is_dubbing = true;
        }
        else if(strcmp(argv[i], "-quiet") == 0){
#ifdef _WIN32
    freopen("nul", "w", stderr);
    freopen("nul", "w", stdout);
#else
    freopen("/dev/null", "w", stderr);
    freopen("/dev/null", "w", stdout);
#endif
        }
        else{
            fprintf(stderr, "Error: Invalid command line flag: %s\n", argv[i]);
            return 1;
        }
    }

    srand(time(NULL));
    const char* memory_file_path = "./output/memory.txt";
    const char* cpu_file_path = "./output/cpu.txt";
    const char* stack_file_path = "./output/stack.txt";
    FILE* stack_file = fopen(stack_file_path, "w");
    if(!stack_file){
        perror(stack_file_path);
        return 1;
    }
    FILE* memory_file = fopen(memory_file_path, "w");
    if(!memory_file){
        fclose(stack_file);
        perror(memory_file_path);
        return 1;
    }
    FILE* cpu_file = fopen(cpu_file_path, "w");
    if(!cpu_file){
        fclose(stack_file);
        fclose(memory_file);
        perror(cpu_file_path);
        return 1;
    }

    Memory memory;
    CPU cpu;

    basic_test(&cpu, &memory, cpu_file, memory_file, stack_file);

    fclose(cpu_file);
    fclose(memory_file);
    fclose(stack_file);

    return 0;
}

void basic_test(CPU* cpu, Memory* memory, FILE* cpu_file, FILE* memory_file, FILE* stack_file){
    
    Memory_init(memory);

    word val1 = 0x1001;
    word val2 = 0x1002;

    word irq_handler_ep = IRQ_HANDLER_ADDRES;
    byte irq_handler[] = {
        rti,
    };

    word nmi_handler_ep = NMI_HANDLER_ADDRES;
    byte nmi_handler[] = {
        rti,
    };

    word hlt_ep = ENTRY_POINT_ADDERS + 0x3000;
    byte hlt[] = {
        0xff, // to halt the cpu
        jmpa, LOW_BYTE(hlt_ep), HIGH_BYTE(hlt_ep), 
    };

    word swap_ep = ENTRY_POINT_ADDERS + 0x2000;
    byte swap[] = {
        ldaa, LOW_BYTE(val1), HIGH_BYTE(val1),    
        tax,         
        ldaa, LOW_BYTE(val2), HIGH_BYTE(val2),   
        staa, LOW_BYTE(val1), HIGH_BYTE(val1),  
        txa,         
        staa, LOW_BYTE(val2), HIGH_BYTE(val2),    
        rts,         
    };

    word _main_ep = ENTRY_POINT_ADDERS + 0x1000;
    byte _main[] = {
        ldai, 0x11,
        staa, LOW_BYTE(val1), HIGH_BYTE(val1),
        ldai, 0x22,
        staa, LOW_BYTE(val2), HIGH_BYTE(val2),
        jsr, LOW_BYTE(swap_ep), HIGH_BYTE(swap_ep),
        rts, 
    };

    word reset_ep = ENTRY_POINT_ADDERS;
    byte reset[] = {
        cli, 
        jsr, LOW_BYTE(_main_ep), HIGH_BYTE(_main_ep),
        jsr, LOW_BYTE(hlt_ep), HIGH_BYTE(hlt_ep),
    };
    
    Memory_load_code(memory, NULL, reset_ep, reset, ARRAY_SIZE(reset));
    Memory_load_code(memory, NULL, _main_ep, _main, ARRAY_SIZE(_main));
    Memory_load_code(memory, NULL, hlt_ep, hlt, ARRAY_SIZE(hlt));
    Memory_load_code(memory, NULL, swap_ep, swap, ARRAY_SIZE(swap));
    Memory_load_code(memory, NULL, nmi_handler_ep, nmi_handler, ARRAY_SIZE(nmi_handler));
    Memory_load_code(memory, NULL, irq_handler_ep, irq_handler, ARRAY_SIZE(irq_handler));

    CPU_reset(cpu, memory);
    while (cpu->run)
    {
        // dubbing:
        if(is_dubbing){
            bool halt = true; 
            char input = '\0';
            while(halt){
                fflush(stdin);
                fprintf(stdout, "(dubbing) Tap a letter only to advance, for help tap `h`\n");
                scanf("%c", &input);
                switch (input)
                {
                case 's':
                    halt = false;
                    break;
                
                case 'c':
                    is_dubbing = false;
                    halt = false;
                    break;

                case 'h':
                    fprintf(stdout, "(dubbing) instructions:\n");
                    fprintf(stdout, "|  `s`: step the progrem to the next commend                   |\n");
                    fprintf(stdout, "|  `h`: List the commands and their charters                   |\n");
                    fprintf(stdout, "|  `c`: Continues program progress until the next breakpoint   |\n");
                    break;
                
                default:
                    fprintf(stdout, "(dubbing) Unknown instruction `%c` please try again\n", input);
                    break;
                }
            }
        }

        CPU_execute(cpu, memory);
        interrupt interrupt_status = trigger_interrupt();
        if(interrupt_status == IRQ) CPU_irq(cpu, memory);
        else if(interrupt_status == NMI) CPU_nmi(cpu, memory);
        else if(interrupt_status == RESET) CPU_reset(cpu, memory);
    } 
    
    Memory_dump_all(memory, memory_file);
    Memory_dump_stack(memory, cpu->SP, stack_file);
    CPU_dump(cpu, cpu_file);

}