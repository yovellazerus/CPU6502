
#include "../include/cpu.h"
#include "../include/instruction.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, COLOR_RED "USAGE: cpu6502 <program.bin>\n");
        fprintf(stderr, "ERROR: no program file was provided\n" COLOR_RESET);
        return 1;
    }
    
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

    FILE* memory_img = fopen(argv[1], "rb");
    if(!memory_img){
        fclose(stack_file);
        fclose(memory_file);
        fclose(cpu_file);
        perror(argv[1]);
        return 1;
    }
    CPU cpu;
    fread(cpu.memory, sizeof(*cpu.memory), MEMORY_SIZE, memory_img);
    fclose(memory_img);

    CPU_reset(&cpu);
    CPU_run(&cpu, false);

    CPU_dump_cpu(&cpu, cpu_file);
    CPU_dump_memory(&cpu, memory_file);
    CPU_dump_stack(&cpu, stack_file);

    fclose(stack_file);
    fclose(memory_file);
    fclose(cpu_file);

    return 0;
}
