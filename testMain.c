
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"

void test_basic();

int main()
{
    test_basic();

    return 0;
}

void test_basic(){
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

    CPU_reset(&cpu, &memory);

    memory.data[0x0000] = 0xa9;
    memory.data[0x0001] = 0xfe;
    memory.data[0x0002] = 0x8d;
    memory.data[0x0003] = 0x34;
    memory.data[0x0004] = 0x12;
    memory.data[0x0005] = 0xaa;
    memory.data[0x0006] = 0x48;
    memory.data[0x0007] = 0xa9;
    memory.data[0x0008] = 0x06;
    memory.data[0x0009] = 0x68;

    memory.data[0x000a] = 0xa9;
    memory.data[0x000b] = 0x48;
    memory.data[0x000c] = 0x48;
    memory.data[0x000d] = 0x48;
    memory.data[0x000e] = 0x48;
    memory.data[0x000f] = 0x48;

    memory.data[0x0010] = 0x20;
    memory.data[0x0011] = 0xab;
    memory.data[0x0012] = 0xcd;
    memory.data[0x0013] = 0xa9;
    memory.data[0x0014] = 0x42;
    memory.data[0x0015] = 0xa9;
    memory.data[0x0016] = 0xfe;
    memory.data[0x0017] = 0xff;

    memory.data[0xcdab] = 0x60;

    while (!cpu.hlt) CPU_execute(&cpu, &memory);
    
    Memory_dump(&memory, memory_file);
    CPU_dump(&cpu, cpu_file);
    
    fclose(cpu_file);
    fclose(memory_file);
}