
#include "../include/cpu.h"
#include "../include/instruction.h"

int main(int argc, char* argv[]){

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

    CPU cpu = {0};
    byte program[] = {
        Opcode_LDA_Immediate, 0xff,
        Opcode_HLT_debug,
    };
    CPU_load_program_from_carr(&cpu, 0, program, ARRAY_SIZE(program));

    while(true){
        // fetch:
        Opcode opcode = cpu.memory[cpu.PC++];

        // decode:
        Instruction instruction = Opcode_to_Instraction_table[opcode];
        if(!instruction){
            CPU_invalid_opcode(&cpu, opcode);
            break; // for now!
        }
        
        // execute:
        instruction(&cpu);
    }

    CPU_dump_cpu(&cpu, cpu_file);
    CPU_dump_memory(&cpu, memory_file);
    CPU_dump_stack(&cpu, stack_file);

    fclose(stack_file);
    fclose(memory_file);
    fclose(cpu_file);

    return 0;
}
