
#include "../include\cpu.h"

void CPU_dump_cpu(CPU * cpu, FILE * file)
{
    fprintf(file, "CPU: {\n");
    fprintf(file, "A: %d (0x%.2x)\n", cpu->A, cpu->A);
    fprintf(file, "X: %d (0x%.2x)\n", cpu->X, cpu->X);
    fprintf(file, "Y: %d (0x%.2x)\n", cpu->Y, cpu->Y);
    fprintf(file, "PC: 0x%.4x\n", cpu->PC);
    fprintf(file, "SP: 0x%.2x\n", cpu->SP);
    fprintf(file, "P {\n");
    fprintf(file, "    n: %d\n", CPU_getFlag(cpu, 'n'));
    fprintf(file, "    v: %d\n", CPU_getFlag(cpu, 'v'));
    fprintf(file, "    u: %d\n", CPU_getFlag(cpu, 'u'));
    fprintf(file, "    b: %d\n", CPU_getFlag(cpu, 'b'));
    fprintf(file, "    d: %d\n", CPU_getFlag(cpu, 'd'));
    fprintf(file, "    i: %d\n", CPU_getFlag(cpu, 'i'));
    fprintf(file, "    z: %d\n", CPU_getFlag(cpu, 'z'));
    fprintf(file, "    c: %d\n", CPU_getFlag(cpu, 'c'));
    fprintf(file, "  }\n");
    fprintf(file, "}\n");
}

void CPU_dump_memory(CPU * cpu, FILE * file)
{
    word line = 0;
    for(int i = 0; i < MEMORY_SIZE; i++){
        if(i % 8 == 0){
            fprintf(file, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(file, " 0x%.2x ", cpu->memory[i]);
    }
}

void CPU_dump_stack(CPU * cpu, FILE * file)
{
    for(int i = STACK_END; i >= STACK_START; i--){
        fprintf(file, "0x%.4x: ", i);
        fprintf(file, " 0x%.2x", cpu->memory[i]);
        if(i == cpu->SP + STACK_START){
            fprintf(file, " <--- sp\n");
        }
        else{
            fprintf(file, "\n");
        }
    }
}

bool CPU_getFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            return (1 & (cpu->P >> 7));
            break;
        case 'v':
            return (1 & (cpu->P >> 6));
            break;
        case 'u': // undefine, on all the time
            return (1 & (cpu->P >> 5));
            break;
        case 'b':
            return (1 & (cpu->P >> 4));
            break;
        case 'd':
            return (1 & (cpu->P >> 3));
            break;
        case 'i':
            return (1 & (cpu->P >> 2));
            break;
        case 'z':
            return (1 & (cpu->P >> 1));
            break;
        case 'c':
            return (1 & (cpu->P >> 0));
            break;
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
            set_color(COLOR_RESET, stderr);
    }
}

bool CPU_onFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->P |= 0x80; // 0b10000000
            break;
        case 'v':
            cpu->P |= 0x40; // 0b01000000
            break;
        case 'u':
            cpu->P |= 0x20; // 0b00100000
            break;
        case 'b':
            cpu->P |= 0x10; // 0b00010000
            break;
        case 'd':
            cpu->P |= 0x08; // 0b00001000
            break;
        case 'i':
            cpu->P |= 0x04;  // 0b00000100
            break;
        case 'z':
            cpu->P |= 0x02; // 0b00000010
            break;
        case 'c':
            cpu->P |= 0x01; // 0b00000001
            break;
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
            set_color(COLOR_RESET, stderr);
    }
    return true;
}

bool CPU_offFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            cpu->P &= ~0x80; // 0b01111111
            break;
        case 'v':
            cpu->P &= ~0x40; // 0b10111111
            break;
        case 'b':
            cpu->P &= ~0x10; // 0b11101111
            break;
        case 'd':
            cpu->P &= ~0x08; // 0b11110111
            break;
        case 'i':
            cpu->P &= ~0x04;  // 0b11111011
            break;
        case 'z':
            cpu->P &= ~0x02; // 0b11111101
            break;
        case 'c':
            cpu->P &= ~0x01; // 0b11111110
            break;
        default:
            set_color(COLOR_RED, stderr);
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
            set_color(COLOR_RESET, stderr);
    }
    return true;
}

void CPU_tick(CPU* cpu, size_t amount){
    return;
}

void CPU_invalid_opcode(CPU * cpu, byte opcode)
{
    set_color(COLOR_RED, stderr);
    fprintf(stderr, "ERROR: unknown opcode (0x%.2x) in addres (0x%.4x)\n", opcode, cpu->PC - 1);
    set_color(COLOR_RESET, stderr);
}

void CPU_load_program_from_carr(CPU* cpu, word entry_point, byte* program, size_t program_size){
    word start = entry_point;
    for(size_t curr = 0; curr < program_size; curr++){
        cpu->memory[start + curr] = program[curr];
    }
}

// done at the hardware level not using normal cpu instructions.
// call wan the reset input is set low, not an interrupt!  
void CPU_reset(CPU *cpu)
{
    cpu->A = UNDEFINED_BYTE();
    cpu->X = UNDEFINED_BYTE();
    cpu->Y = UNDEFINED_BYTE();
    cpu->SP = SP_INIT_VALUE;
    cpu->P = RESET_P_REGISTER; 

    CPU_tick(cpu, 6);

    word bus;
    bus = cpu->memory[RESET_VECTOR_LOW_ADDER];
    CPU_tick(cpu, 1);
    bus += cpu->memory[RESET_VECTOR_HIGH_ADDER] * 0x0100;
    CPU_tick(cpu, 1);
    cpu->PC = bus;
    CPU_tick(cpu, 1); 
}
