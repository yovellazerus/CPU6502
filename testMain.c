
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

void test_basic_memory_cpu_dump();
void test_basic_operation_LDA_Immediate();


int main()
{

    //test_basic_memory_cpu_dump();
    test_basic_operation_LDA_Immediate();

    return 0;
}

void test_basic_memory_cpu_dump(){
    const char* memory_file_path = "./output/memory.txt";
    const char* cpu_file_path = "./output/cpu.txt";
    FILE* memory_file = fopen(memory_file_path, "w");
    if(!memory_file){
        perror(memory_file_path);
        exit(1);
    }
    FILE* cpu_file = fopen(cpu_file_path, "w");
    if(!cpu_file){
        fclose(memory_file);
        perror(cpu_file_path);
        exit(1);
    }

    Memory memory;
    CPU cpu;
    Memory_init(&memory);
    CPU_init(&cpu, NULL);
    
    Memory_dump(&memory, memory_file);
    CPU_onFlag(&cpu, 'i');
    CPU_onFlag(&cpu, 'b');
    CPU_onFlag(&cpu, 'v');
    CPU_onFlag(&cpu, 'n');
    CPU_onFlag(&cpu, 'z');
    CPU_offFlag(&cpu, 'i');
    CPU_offFlag(&cpu, 'v');
    CPU_dump(&cpu, cpu_file);
    
    fclose(cpu_file);
    fclose(memory_file);
}

void test_basic_operation_LDA_Immediate(){
    const char* memory_file_path = "./output/memory.txt";
    const char* cpu_file_path = "./output/cpu.txt";
    FILE* memory_file = fopen(memory_file_path, "w");
    if(!memory_file){
        perror(memory_file_path);
        exit(1);
    }
    FILE* cpu_file = fopen(cpu_file_path, "w");
    if(!cpu_file){
        fclose(memory_file);
        perror(cpu_file_path);
        exit(1);
    }

    Memory memory;
    CPU cpu;
    Memory_init(&memory);
    CPU_init(&cpu, NULL);

    memory.data[0x0000] = 0xad;
    memory.data[0x0001] = 0x34;
    memory.data[0x0002] = 0x12;
    memory.data[0x1234] = 0xff;

    CPU_execute(&cpu, &memory);
    
    Memory_dump(&memory, memory_file);
    CPU_dump(&cpu, cpu_file);
    
    fclose(cpu_file);
    fclose(memory_file);
}
