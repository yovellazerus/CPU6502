
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "CPU.h"
#include "Memory.h"


int main()
{
    FILE* memory_file = fopen("./output/memory.txt", "w");
    if(!memory_file){
        perror("./output/memory.txt");
        return 1;
    }
    FILE* cpu_file = fopen("./output/cpu.txt", "w");
    if(!cpu_file){
        fclose(cpu_file);
        perror("./output/cpu.txt");
        return 1;
    }
    Memory memory;
    CPU cpu;
    Memory_init(&memory);
    CPU_init(&cpu);
    
    CPU_execute(&cpu, &memory);
    
    Memory_dump(&memory, memory_file);
    CPU_onFlag(&cpu, 'i');
    CPU_dump(&cpu, cpu_file);
    
    
    fclose(cpu_file);
    fclose(memory_file);
    
    return 0;
}

