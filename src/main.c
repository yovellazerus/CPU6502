
#include "../include/cpu.h"
#include "../include/instruction.h"

int main(int argc, char* argv[]){
    (void) argc;
    (void) argv;

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

    CPU cpu;

    // for now ROM and RAM are the same memory component, this is not true in actual hardware...
    cpu.memory[RESET_VECTOR_HIGH_ADDER] = HIGH_BYTE(RESET_HANDLER);
    cpu.memory[RESET_VECTOR_LOW_ADDER] = LOW_BYTE(RESET_HANDLER);

    cpu.memory[IRQ_VECTOR_HIGH_ADDER] = HIGH_BYTE(IRQ_HANDLER);
    cpu.memory[IRQ_VECTOR_LOW_ADDER] = LOW_BYTE(IRQ_HANDLER);

    cpu.memory[NMI_VECTOR_HIGH_ADDER] = HIGH_BYTE(NMI_HANDLER);
    cpu.memory[NMI_VECTOR_LOW_ADDER] = LOW_BYTE(NMI_HANDLER);

    cpu.memory[IRQ_HANDLER] = Opcode_RTI;
    cpu.memory[NMI_HANDLER] = Opcode_RTI;

    byte program[] = {
        Opcode_LDA_Immediate,  0x22,   // LDA #$22
        Opcode_STA_ZeroPage,   0x00,   // STA $00
        Opcode_LDA_Immediate,  0x11,   // LDA #$11
        Opcode_ADC_ZeroPage,   0x00,   // ADC $00

        0xff,                               // HLT
    };

    CPU_load_program_from_carr(&cpu, RESET_HANDLER, program, ARRAY_SIZE(program));

    CPU_reset(&cpu); // hard coded for now
    
    CPU_run(&cpu);

    CPU_dump_cpu(&cpu, cpu_file);
    CPU_dump_memory(&cpu, memory_file);
    CPU_dump_stack(&cpu, stack_file);

    fclose(stack_file);
    fclose(memory_file);
    fclose(cpu_file);

    return 0;
}
