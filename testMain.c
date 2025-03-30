
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

#define LABEL_LOW(LABEL) (LABEL % 0x0100)
#define LABEL_HIGH(LABEL) (LABEL / 0x0100)

#define LOAD_LABEL(LABEL, MEMORY) (MEMORY->label_table[LABEL] = #LABEL)

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
    
    CPU_init(cpu, NULL);
    Memory_init(memory);
    CPU_reset(cpu, memory);

    byte code[] = {0xa9, 0xfe, 0x8d, 0x34, 0x12,
                    0xaa, 0x48, 0xa9, 0x06 ,0xa9, 0x48,
                    0x48, 0x48, 0x48 ,0x20, 0x00, 0x40, 
                    0xa9, 0x42, 0xa9, 0xfe, 0xff,
    };
    byte foo[] = {
        0xa9, 0x00, 0xf0, 0x05, 0xa9, 0x01, 0x8d, 0x00, 0x20, 0x60,
    };

    Memory_load_code(memory, "test_program", GLOBAL_START, code, ARRAY_SIZE(code));
    Memory_load_code(memory, "foo", 0x4000, foo, ARRAY_SIZE(foo));

    while (!cpu->hlt) CPU_execute(cpu, memory);
    
    Memory_dump_all(memory, memory_file);
    Memory_dump_stack(memory, cpu->SP, stack_file);
    CPU_dump(cpu, cpu_file);

}