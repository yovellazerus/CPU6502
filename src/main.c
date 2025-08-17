
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

    // for now ROM and RAM are the same memory component, this is not true in actual implementation...
    // this is ROM:
    cpu.memory[RESET_VECTOR_HIGH_ADDER] = HIGH_BYTE(ENTRY_POINT_ADDERS);
    cpu.memory[RESET_VECTOR_LOW_ADDER] = LOW_BYTE(ENTRY_POINT_ADDERS);

    cpu.memory[IRQ_VECTOR_HIGH_ADDER] = HIGH_BYTE(IRQ_HANDLER_ADDRES);
    cpu.memory[IRQ_VECTOR_LOW_ADDER] = LOW_BYTE(IRQ_HANDLER_ADDRES);

    cpu.memory[NMI_VECTOR_HIGH_ADDER] = HIGH_BYTE(NMI_HANDLER_ADDRES);
    cpu.memory[NMI_VECTOR_LOW_ADDER] = LOW_BYTE(NMI_HANDLER_ADDRES);

    cpu.memory[IRQ_HANDLER_ADDRES] = Opcode_RTI;


    byte mul_6502_program[] = {
    // Clear result = 0
    0xA9, 0x00,       // LDA #$00
    0x85, 0x12,       // STA $12 (res_lo)
    0x85, 0x13,       // STA $13 (res_hi)

    // Copy multiplier ($11) into counter ($14)
    0xA5, 0x11,       // LDA $11
    0x85, 0x14,       // STA $14 (counter)

    // MultiplyLoop:
    0xA5, 0x14,       // LDA $14
    0xF0, 0x15,       // BEQ Done (if counter==0)

    // res_lo += multiplicand
    0xA5, 0x12,       // LDA $12
    0x18,             // CLC
    0x65, 0x10,       // ADC $10 (multiplicand)
    0x85, 0x12,       // STA $12

    // res_hi += carry
    0xA5, 0x13,       // LDA $13
    0x69, 0x00,       // ADC #$00
    0x85, 0x13,       // STA $13

    // counter--
    0xC6, 0x14,       // DEC $14
    0x4C, 0x06, 0x06, // JMP MultiplyLoop (absolute)

    // Done:
    0x00              // BRK
    };

    CPU_load_program_from_carr(&cpu, ENTRY_POINT_ADDERS, mul_6502_program, ARRAY_SIZE(mul_6502_program));

    CPU_reset(&cpu);
    while(true){
        // fetch:
        Opcode opcode = cpu.memory[cpu.PC++];

        // decode:
        Instruction instruction = Opcode_to_Instruction_table[opcode];
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
