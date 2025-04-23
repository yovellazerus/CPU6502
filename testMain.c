
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

#include <conio.h>

#define NO_INT 0
#define NMI 1
#define IRQ 2
#define RESET 3

// very basic input system for now and not portable....
int trigger_interrupt(){
    if (_kbhit()) {
        char c = _getch();
        if(c == 'q'){
            return NMI;
        }
        else if(c == 'r'){
            return RESET;
        }
        else{
            return IRQ;
        }
    }
    return NO_INT;
}

void test_basic(CPU* cpu, Memory* memory, FILE* cpu_file, FILE* memory_file, FILE* stack_file);

int main()
{
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

    test_basic(&cpu, &memory, cpu_file, memory_file, stack_file);

    fclose(cpu_file);
    fclose(memory_file);
    fclose(stack_file);

    return 0;
}

void test_basic(CPU* cpu, Memory* memory, FILE* cpu_file, FILE* memory_file, FILE* stack_file){
    
    Memory_init(memory);
    
    word foo_addr = 0x4000;
    word res = 0x2000;
    byte foo[] = {
        ldai, 0x01, 
        beq, 0x05, 
        ldai, 0x01, 
        staa, LOW_BYTE(res), HIGH_BYTE(res), 
        rts,
    };

    word var = 0x1000;
    byte code[] = {
        ldai, 0xfe, 
        staa, LOW_BYTE(var), HIGH_BYTE(var),
        tax, 
        pha, 
        ldai, 0x06, 
        ldai, 0x48,
        pha, 
        pha, 
        pha, 
        jsr, LOW_BYTE(foo_addr), HIGH_BYTE(foo_addr), 
        ldai, 0x42, 
        ldai, 0xfe, 
        brk,
        0xff, // to stop the cpu
    };
    

    Memory_load_code(memory, NAME(code), ENTRY_POINT_ADDERS, code, ARRAY_SIZE(code));
    Memory_load_code(memory, NAME(foo), foo_addr, foo, ARRAY_SIZE(foo));
    LOAD_LABEL(memory, res);
    LOAD_LABEL(memory, var);

    CPU_reset(cpu, memory);
    while (cpu->run)
    {
        CPU_execute(cpu, memory);
        int interrupt_status = trigger_interrupt();
        if(interrupt_status == IRQ) CPU_generate_irq(cpu, memory);
        else if(interrupt_status == NMI) CPU_generate_nmi(cpu, memory);
        else if(interrupt_status == RESET) CPU_reset(cpu, memory);
    } 
    
    Memory_dump_all(memory, memory_file);
    Memory_dump_stack(memory, cpu->SP, stack_file);
    CPU_dump(cpu, cpu_file);

}