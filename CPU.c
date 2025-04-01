
#include "CPU.h"

void CPU_init(CPU* cpu, const char* name){
    cpu->name = name;
    cpu->cycles = 0;
}

// TODO: The order of the flags is probably reversed...
bool CPU_getFlag(CPU* cpu, char flag){
    switch (flag){
        case 'n':
            return (1 & (cpu->P >> 7));
            break;
        case 'v':
            return (1 & (cpu->P >> 6));
            break;
        case 'u':
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
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
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
            fprintf(stderr, "ERROR: unknown cpu flag '%c'\n", flag);
            return false;
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
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            return false;
    }
    return true;
}

void CPU_dump(CPU* cpu, FILE* stream){
    fprintf(stream, "CPU: %s {\n", cpu->name ? cpu->name : "");
    fprintf(stream, "A: %d (0x%.2x)\n", cpu->A, cpu->A);
    fprintf(stream, "X: %d (0x%.2x)\n", cpu->X, cpu->X);
    fprintf(stream, "Y: %d (0x%.2x)\n", cpu->Y, cpu->Y);
    fprintf(stream, "PC: 0x%.4x\n", cpu->PC);
    fprintf(stream, "SP: 0x%.2x\n", cpu->SP);
    fprintf(stream, "P {\n");
    fprintf(stream, "    n: %d\n", CPU_getFlag(cpu, 'n'));
    fprintf(stream, "    v: %d\n", CPU_getFlag(cpu, 'v'));
    fprintf(stream, "    u: %d\n", CPU_getFlag(cpu, 'u'));
    fprintf(stream, "    b: %d\n", CPU_getFlag(cpu, 'b'));
    fprintf(stream, "    d: %d\n", CPU_getFlag(cpu, 'd'));
    fprintf(stream, "    i: %d\n", CPU_getFlag(cpu, 'i'));
    fprintf(stream, "    z: %d\n", CPU_getFlag(cpu, 'z'));
    fprintf(stream, "    c: %d\n", CPU_getFlag(cpu, 'c'));
    fprintf(stream, "  }\n");
    fprintf(stream, "cycles: %zu\n", cpu->cycles);
    fprintf(stream, "}\n");
}

void CPU_execute(CPU* cpu, Memory* memory){
    byte opcode = memory->data[cpu->PC];
    printf("0x%.4x: 0x%.2x\n", cpu->PC, opcode); // debug
    cpu->PC++;
    operation op = operation_table[opcode];
    if(op){
        op(cpu, memory);
    }
    else{
        CPU_invalid_opcode(cpu, opcode);
    }
}


// done at the hardware level not using normal cpu instructions.
// call wan the reset input is set low, not an interrupt!  
void CPU_reset(CPU *cpu, Memory *memory)
{
    cpu->A = UNDEFINED_BYTE();
    cpu->X = UNDEFINED_BYTE();
    cpu->Y = UNDEFINED_BYTE();
    cpu->SP = SP_INIT_VALUE;
    cpu->P = RESET_P_REGISTER; 
    cpu->hlt = false;

    CPU_tick(cpu, 6);

    word bus;
    bus = memory->data[RESET_VECTOR_LOW_BYTE];
    CPU_tick(cpu, 1);
    bus += memory->data[RESET_VECTOR_HIGH_BYTE] * 0x0100;
    CPU_tick(cpu, 1);
    printf("0x%.4x\n", bus);
    cpu->PC = bus;
    CPU_tick(cpu, 1); 

}

void CPU_tick(CPU* cpu, size_t amount){
    cpu->cycles += amount;
    // TODO: add sleep or something to simulate real cpu execution time
}

void CPU_invalid_opcode(CPU* cpu, byte opcode){
    // 6502 invalid opcode is like NOP
    CPU_tick(cpu, 2);
    // for dubbing only!
    fprintf(stderr, "ERROR: unknown opcode (0x%.2x) in addres (0x%.4x)\n", opcode, cpu->PC - 1);
    cpu->hlt = true;
}