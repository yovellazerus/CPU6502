
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

    byte foo[] = {
        Opcode_LDA_Immediate, 0x12,
        Opcode_STA_Absolute, 0x00, 0x20,
        Opcode_LDA_Immediate, 0x42,
        Opcode_SEC,
        Opcode_RTS,
    };
    word foo_entry = 0x9000;

    byte program[] = {
        Opcode_LDA_Immediate,  0x22,                // LDA #$22
        Opcode_STA_ZeroPage,   0x00,                // STA $00

        Opcode_TAX,
        Opcode_TAY,
        
        Opcode_PHP,
        Opcode_PHP,

        Opcode_JSR,  LOW_BYTE(foo_entry), HIGH_BYTE(foo_entry),

        Opcode_BRK, 0x01,

        Opcode_NOP,

        Opcode_NOP,

        0xff,                                       // HLT
    };

    CPU_load_program_from_carr(&cpu, RESET_HANDLER, program, ARRAY_SIZE(program));
    CPU_load_program_from_carr(&cpu, foo_entry, foo, ARRAY_SIZE(foo));
    CPU_dumpProgram(&cpu, RESET_HANDLER, ARRAY_SIZE(program), stdout);
    CPU_dumpProgram(&cpu, foo_entry, ARRAY_SIZE(foo), stdout);
    printf("\n");

    CPU_reset(&cpu); // hard coded for now
    
    CPU_run(&cpu, true);

    CPU_dump_cpu(&cpu, cpu_file);
    CPU_dump_memory(&cpu, memory_file);
    CPU_dump_stack(&cpu, stack_file);

    fclose(stack_file);
    fclose(memory_file);
    fclose(cpu_file);

    return 0;
}
