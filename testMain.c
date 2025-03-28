
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

#define LABEL_LOW(LABEL) (LABEL % 0x0100)
#define LABEL_HIGH(LABEL) (LABEL / 0x0100)

#define LOAD_LABEL(LABEL, MEMORY) (MEMORY.label_table[LABEL] = #LABEL)

void test_basic();

int main()
{
    test_basic();

    return 0;
}

void test_basic(){
    const char* memory_file_path = "./output/memory.txt";
    const char* cpu_file_path = "./output/cpu.txt";
    const char* stack_file_path = "./output/stack.txt";
    FILE* stack_file = fopen(stack_file_path, "w");
    if(!stack_file){
        perror(stack_file_path);
        exit(1);
    }
    FILE* memory_file = fopen(memory_file_path, "w");
    if(!memory_file){
        fclose(stack_file);
        perror(memory_file_path);
        exit(1);
    }
    FILE* cpu_file = fopen(cpu_file_path, "w");
    if(!cpu_file){
        fclose(stack_file);
        fclose(memory_file);
        perror(cpu_file_path);
        exit(1);
    }

    Memory memory;
    CPU cpu;
    word entry_point = 0x1000;

    Memory_init(&memory, entry_point);
    CPU_init(&cpu, NULL);
    CPU_reset(&cpu, &memory);

    byte code1[] = {0xa9, 0xfe, 0x8d, 0x34, 0x12,
                    0xaa, 0x48, 0xa9, 0x06 ,0xa9, 0x48,
                    0x48, 0x48, 0x48 ,0x20, 0x00, 0x40, 
                    0xa9, 0x42, 0xa9, 0xfe, 0xff,
    };
    byte function1[] = {
        0xa9, 0x00, 0xf0, 0x05, 0xa9, 0x01, 0x8d, 0x00, 0x20, 0x60,
    };

    word res_AND = 0x2000;
    LOAD_LABEL(res_AND, memory);

    byte test_AND[] = {
        0xa9, 0xab, 0x8d, LABEL_LOW(res_AND), LABEL_HIGH(res_AND),
        0x29, 0x0f,
    };

    Memory_load_code(&memory, "test_AND", entry_point, test_AND, ARRAY_SIZE(test_AND));

    // Memory_load_code(&memory, "test_program", entry_point, code1, ARRAY_SIZE(code1));
    // Memory_load_code(&memory, "function1", 0x4000, function1, ARRAY_SIZE(function1));

    while (!cpu.hlt) CPU_execute(&cpu, &memory);
    
    Memory_dump_all(&memory, memory_file);
    Memory_dump_stack(&memory, cpu.SP, stack_file);
    CPU_dump(&cpu, cpu_file);
    
    fclose(cpu_file);
    fclose(memory_file);
    fclose(stack_file);
    
}