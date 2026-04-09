
#include "cpu.h"

struct CPU_t
{
    byte A;
    byte X;
    byte Y;
    byte SP;
    byte P;
    word PC;

    // memory read/write abstraction
    Bus_t bus;
};

static void setFlag(CPU_t* cpu, char flag, bool value)
{
    switch(flag) {
        case 'c':
            if(value) cpu->P |= 0x01;
            else      cpu->P &= ~0x01;
            break;
        case 'z':
            if(value) cpu->P |= 0x02;
            else      cpu->P &= ~0x02;
            break;
        case 'i':
            if(value) cpu->P |= 0x04;
            else      cpu->P &= ~0x04;
            break;
        case 'd':
            if(value) cpu->P |= 0x08;
            else      cpu->P &= ~0x08;
            break;
        case 'u': // undefine, on all the time
            if(value) cpu->P |= 0x20;
            else      cpu->P &= ~0x20;
            break;
        case 'b':
            if(value) cpu->P |= 0x10;
            else      cpu->P &= ~0x10;
            break;
        case 'v':
            if(value) cpu->P |= 0x40;
            else      cpu->P &= ~0x40;
            break;
        case 'n':
            if(value) cpu->P |= 0x80;
            else      cpu->P &= ~0x80;
            break;
        default:
            CPU_ERROR(cpu, "unknown cpu flag %c\n", flag);
            break;
    }
}

static bool getFlag(CPU_t* cpu, char flag){
    switch (flag){
        case 'n':
            return (1 & (cpu->P >> 7));
        case 'v':
            return (1 & (cpu->P >> 6));
        case 'u': // undefine, on all the time
            return (1 & (cpu->P >> 5));
        case 'b':
            return (1 & (cpu->P >> 4));
        case 'd':
            return (1 & (cpu->P >> 3));
        case 'i':
            return (1 & (cpu->P >> 2));
        case 'z':
            return (1 & (cpu->P >> 1));
        case 'c':
            return (1 & (cpu->P >> 0));
        default:
            CPU_ERROR(cpu, "unknown cpu flag %c\n", flag);
            return false;
    }
}

static void  update_NZCV_add(CPU_t* cpu, byte a, byte b, byte r){
    if(!cpu) return;
    setFlag(cpu, 'z', !r);
	setFlag(cpu, 'n', r >> 7);
	setFlag(cpu, 'c', ((word)a + b) >> 8);
	setFlag(cpu, 'v', (~(a ^ b) & (a ^ r)) >> 7);
}

static void  update_NZCV_sub(CPU_t* cpu, byte a, byte b, byte r){
    if(!cpu) return;
    setFlag(cpu, 'z', r == 0);
	setFlag(cpu, 'n', r >> 7);
	setFlag(cpu, 'c', a >= b);
	setFlag(cpu, 'v', ((a ^ b) & (a ^ r)) >> 7);
}

static void  update_NZ(CPU_t* cpu, byte r){
    if(!cpu) return;
    setFlag(cpu, 'z', !r);
	setFlag(cpu, 'n', r >> 7);
}

// TODO: fix bugs in zero page mods
void helper_load(CPU_t* cpu, char reg, Addressing_mode_t amod){
    byte imm = 0;
    word addr = 0;
    byte indirect = 0;
    if(reg == 'A'){
        switch (amod)
        {
        case Addressing_mode_Immediate:
            imm = CPU_read(cpu, cpu->PC++);
            cpu->A = imm;
            break;
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->A = CPU_read(cpu, addr);
            
            break;
        case Addressing_mode_AbsoluteX:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->A = CPU_read(cpu, addr + cpu->X);
            break;
        case Addressing_mode_AbsoluteY:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->A = CPU_read(cpu, addr + cpu->Y);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->A = CPU_read(cpu, addr);
            break;
        case Addressing_mode_ZeroPageX:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->A = CPU_read(cpu, (addr + cpu->X) & 0xFF);
            break;
        case Addressing_mode_IndirectX:
            indirect = CPU_read(cpu, cpu->PC++);
            indirect += cpu->X;
            addr = CPU_read(cpu, indirect);
            addr += CPU_read(cpu, indirect + 1) << 8;
            cpu->A = CPU_read(cpu, addr);
            break;
        case Addressing_mode_IndirectY:
            indirect = CPU_read(cpu, cpu->PC++);
            addr = CPU_read(cpu, indirect);
            addr += CPU_read(cpu, indirect + 1) << 8;
            cpu->A = CPU_read(cpu, addr + cpu->Y);
            break;
        
        default:
            CPU_ERROR(cpu, "unknown addresing mode for register A");
            break;
        }
        update_NZ(cpu, cpu->A);
    }
    else if(reg == 'X'){
        
        switch (amod)
        {
        case Addressing_mode_Immediate:
            imm = CPU_read(cpu, cpu->PC++);
            cpu->X = imm;
            break;
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->X = CPU_read(cpu, addr);
            break;
        case Addressing_mode_AbsoluteY:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->X = CPU_read(cpu, addr + cpu->Y);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->X = CPU_read(cpu, addr);
            break;
        case Addressing_mode_ZeroPageY:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->X = CPU_read(cpu, (addr + cpu->Y) & 0xFF);
            break;
    
        default:
            CPU_ERROR(cpu, "unknown addresing mode for register X");
            break;
        }
        update_NZ(cpu, cpu->X);
    }
    else if(reg == 'Y'){
        
        switch (amod)
        {
        case Addressing_mode_Immediate:
            imm = CPU_read(cpu, cpu->PC++);
            cpu->Y = imm;
            break;
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->Y = CPU_read(cpu, addr);
            break;
        case Addressing_mode_AbsoluteX:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            cpu->Y = CPU_read(cpu, addr + cpu->X);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->Y = CPU_read(cpu, addr);;
            break;
        case Addressing_mode_ZeroPageX:
            addr = CPU_read(cpu, cpu->PC++);
            cpu->Y = CPU_read(cpu, (addr + cpu->X) & 0xFF);
            break;
    
        default:
            CPU_ERROR(cpu, "unknown addresing mode for register Y");
            break;
        }
        update_NZ(cpu, cpu->Y);
    }
    else{
        CPU_ERROR(cpu, "invalid register: \"%c\" for load operation", reg);
    }
}

// TODO: fix bugs in zero page mods
void helper_store(CPU_t* cpu, char reg, Addressing_mode_t amod){
    byte imm = 0;
    word addr = 0;
    byte indirect = 0;
    if(reg == 'A'){
    
        switch (amod)
        {
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            CPU_write(cpu, addr, cpu->A);
            break;
        case Addressing_mode_AbsoluteX:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            CPU_write(cpu, addr + cpu->X, cpu->A);
            break;
        case Addressing_mode_AbsoluteY:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            CPU_write(cpu, addr + cpu->Y, cpu->A);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            CPU_write(cpu, addr, cpu->A);
            break;
        case Addressing_mode_ZeroPageX:
            CPU_write(cpu, cpu->X + cpu->PC++, cpu->A);
            break;
        case Addressing_mode_IndirectX:
            indirect = CPU_read(cpu, cpu->PC++);
            indirect += cpu->X;
            addr = CPU_read(cpu, indirect);
            addr += CPU_read(cpu, indirect + 1) << 8;
            CPU_write(cpu, addr, cpu->A);
            break;
        case Addressing_mode_IndirectY:
            indirect = CPU_read(cpu, cpu->PC++);
            addr = CPU_read(cpu, indirect);
            addr += CPU_read(cpu, indirect + 1) << 8;
            CPU_write(cpu, addr + cpu->Y, cpu->A);
            break;
        
        default:
            CPU_ERROR(cpu, "invalid addresing mode for store operation for register A");
            break;
        }
    }
    else if(reg == 'X'){
        switch (amod)
        {
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            CPU_write(cpu, addr, cpu->X);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            CPU_write(cpu, addr, cpu->X);
            break;
        case Addressing_mode_ZeroPageY:
            CPU_write(cpu, cpu->Y + cpu->PC++, cpu->X);
            break;
    
        default:
            CPU_ERROR(cpu, "invalid addresing mode for store operation for register X");
            break;
        }
    }
    else if(reg == 'Y'){
        switch (amod)
        {
        case Addressing_mode_Absolute:
            addr = CPU_read(cpu, cpu->PC++);
            addr += CPU_read(cpu, cpu->PC++) << 8;
            CPU_write(cpu, addr, cpu->Y);
            break;
        case Addressing_mode_ZeroPage:
            addr = CPU_read(cpu, cpu->PC++);
            CPU_write(cpu, addr, cpu->Y);
            break;
        case Addressing_mode_ZeroPageX:
            CPU_write(cpu, cpu->X + cpu->PC++, cpu->Y);
            break;
    
        default:
            CPU_ERROR(cpu, "invalid addresing mode for store operation for register Y");
            break;
        }
    }
    else{
        CPU_ERROR(cpu, "invalid register: \"%c\" for store operation", reg);
    }
}

void helper_and(CPU_t* cpu, Addressing_mode_t amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Addressing_mode_Immediate:
        imm = CPU_read(cpu, cpu->PC++);
        cpu->A &= imm;
        
        break;
    case Addressing_mode_ZeroPage:
        addr = CPU_read(cpu, cpu->PC++);
        cpu->A &= CPU_read(cpu, addr);
        break;
    case Addressing_mode_ZeroPageX:
        addr = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        cpu->A &= CPU_read(cpu, addr);
        break;
    case Addressing_mode_Absolute:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A &= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_AbsoluteX:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A &= CPU_read(cpu, addr + cpu->X);
        break;
    case Addressing_mode_AbsoluteY:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A &= CPU_read(cpu, addr + cpu->Y);
        break;
    case Addressing_mode_IndirectX:
        indirect = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        addr = CPU_read(cpu, indirect);
        addr |= CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A &= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_IndirectY:
        indirect = CPU_read(cpu, cpu->PC++);
        addr = CPU_read(cpu, indirect);
        addr += CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A &= CPU_read(cpu, addr + cpu->Y);
        break;
    
    default:
        CPU_ERROR(cpu, "invalid Addresing Mod for and instruction");
        break;
    }
    update_NZ(cpu, cpu->A);
}

void helper_eor(CPU_t* cpu, Addressing_mode_t amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Addressing_mode_Immediate:
        imm = CPU_read(cpu, cpu->PC++);
        cpu->A ^= imm;
        
        break;
    case Addressing_mode_ZeroPage:
        addr = CPU_read(cpu, cpu->PC++);
        cpu->A ^= CPU_read(cpu, addr);
        break;
    case Addressing_mode_ZeroPageX:
        addr = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        cpu->A ^= CPU_read(cpu, addr);
        break;
    case Addressing_mode_Absolute:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A ^= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_AbsoluteX:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A ^= CPU_read(cpu, addr + cpu->X);
        break;
    case Addressing_mode_AbsoluteY:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A ^= CPU_read(cpu, addr + cpu->Y);
        break;
    case Addressing_mode_IndirectX:
        indirect = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        addr = CPU_read(cpu, indirect);
        addr |= CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A ^= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_IndirectY:
        indirect = CPU_read(cpu, cpu->PC++);
        addr = CPU_read(cpu, indirect);
        addr += CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A ^= CPU_read(cpu, addr + cpu->Y);
        break;
    
    default:
        CPU_ERROR(cpu, "invalid Addresing Mod for and instruction");
        break;
    }
    update_NZ(cpu, cpu->A);
}

void helper_ora(CPU_t* cpu, Addressing_mode_t amod){
    byte imm = 0;
    word addr = 0;
    word indirect = 0;
    switch (amod)
    {
    case Addressing_mode_Immediate:
        imm = CPU_read(cpu, cpu->PC++);
        cpu->A |= imm;
        
        break;
    case Addressing_mode_ZeroPage:
        addr = CPU_read(cpu, cpu->PC++);
        cpu->A |= CPU_read(cpu, addr);
        break;
    case Addressing_mode_ZeroPageX:
        addr = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        cpu->A |= CPU_read(cpu, addr);
        break;
    case Addressing_mode_Absolute:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A |= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_AbsoluteX:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A |= CPU_read(cpu, addr + cpu->X);
        break;
    case Addressing_mode_AbsoluteY:
        addr = CPU_read(cpu, cpu->PC++);
        addr += CPU_read(cpu, cpu->PC++) << 8;
        cpu->A |= CPU_read(cpu, addr + cpu->Y);
        break;
    case Addressing_mode_IndirectX:
        indirect = (CPU_read(cpu, cpu->PC++) + cpu->X) & 0xFF;
        addr = CPU_read(cpu, indirect);
        addr |= CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A |= CPU_read(cpu, addr);
        
        break;
    case Addressing_mode_IndirectY:
        indirect = CPU_read(cpu, cpu->PC++);
        addr = CPU_read(cpu, indirect);
        addr += CPU_read(cpu, (indirect + 1) & 0xFF) << 8;
        cpu->A |= CPU_read(cpu, addr + cpu->Y);
        break;
    
    default:
        CPU_ERROR(cpu, "invalid Addresing Mod for and instruction");
        break;
    }
    update_NZ(cpu, cpu->A);
}

byte get_register(CPU_t* cpu, char reg){
    switch (reg)
    {
    case 'A':
        return cpu->A;
    case 'X':
        return cpu->X;
    case 'Y':
        return cpu->Y;
    case 'S':
        return cpu->SP;
    case 'P':
        return cpu->P;
    default:
        CPU_ERROR(cpu, "unknown register: \"%c\"", reg);
        break;
    }
}

void set_register(CPU_t* cpu, char reg, byte value){
    switch (reg)
    {
    case 'A':
        cpu->A = value;
    case 'X':
        cpu->X = value;
    case 'Y':
        cpu->Y = value;
    case 'S':
        cpu->SP = value;
    case 'P':
        cpu->P = value;
    default:
        CPU_ERROR(cpu, "unknown register: \"%c\"", reg);
        return;
    }
    if(reg != 'S' && reg != 'P') update_NZ(cpu, value);
}

void push(CPU_t* cpu, byte value){
    if(!cpu) return;
    CPU_write(cpu, 0x0100 + --cpu->SP, value);
}

byte pop(CPU_t* cpu){
    if(!cpu) return 0xff;
    return CPU_read(cpu, 0x0100 + cpu->SP++);
}

// user interface:

CPU_t* CPU_create(Bus_t bus){
    if(!bus.read || !bus.write) return NULL;
    CPU_t* res = (CPU_t*)malloc(sizeof(*res));
    if(!res) return NULL;
    memset(res, 0, sizeof(*res));
    res->bus = bus;
    return res;
}

void CPU_destroy(CPU_t* cpu){
    if(!cpu) return;
    free(cpu);
}

byte CPU_read(CPU_t* cpu, word addr){
    if(!cpu) return 0xff; // open bus
    return cpu->bus.read(cpu->bus.ctx, addr);
}

void CPU_write(CPU_t* cpu, word addr, byte value){
    if(!cpu) return;
    cpu->bus.write(cpu->bus.ctx, addr, value);
}

bool CPU_step(CPU_t* cpu){
    if(!cpu) return false;
    // fetch:
    Opcode_t opcode = CPU_read(cpu, cpu->PC++);
    // decode:
    Instruction_t instruction = Opcode_to_Instruction_table[opcode];
    if(!instruction){
        CPU_ERROR(cpu, "unknown opcode: 0x%.2x in PC: 0x%.4x", opcode, cpu->PC - 1);
        return false;
    }
    // execute:
    instruction(cpu);
    return true;
}

void CPU_error(CPU_t* cpu, const char* fmt, ...){
    if(!cpu || !fmt) return;
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, COLOR_RED "ERROR: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " " COLOR_RESET);
    va_end(args);
}

void  CPU_dump_registers(CPU_t* cpu, FILE* file){
    if(!cpu || !file) return;
    fprintf(file, "CPU: {\n");
    fprintf(file, "A: %d (0x%.2x)\n", cpu->A, cpu->A);
    fprintf(file, "X: %d (0x%.2x)\n", cpu->X, cpu->X);
    fprintf(file, "Y: %d (0x%.2x)\n", cpu->Y, cpu->Y);
    fprintf(file, "PC: 0x%.4x\n", cpu->PC);
    fprintf(file, "SP: 0x%.2x\n", cpu->SP);
    fprintf(file, "P (0x%.2x) {\n", cpu->P);
    fprintf(file, "    n: %d\n", getFlag(cpu, 'n'));
    fprintf(file, "    v: %d\n", getFlag(cpu, 'v'));
    fprintf(file, "    u: %d\n", getFlag(cpu, 'u'));
    fprintf(file, "    b: %d\n", getFlag(cpu, 'b'));
    fprintf(file, "    d: %d\n", getFlag(cpu, 'd'));
    fprintf(file, "    i: %d\n", getFlag(cpu, 'i'));
    fprintf(file, "    z: %d\n", getFlag(cpu, 'z'));
    fprintf(file, "    c: %d\n", getFlag(cpu, 'c'));
    fprintf(file, "  }\n");
    fprintf(file, "}\n");
}

void CPU_reset(CPU_t* cpu)
{
    if(!cpu) return;
    cpu->SP = 0xfd;
    setFlag(cpu, 'u', true);
    setFlag(cpu, 'i', true);
    setFlag(cpu, 'd', false);

    word addr;
    addr = CPU_read(cpu, 0xfffc);
    addr += CPU_read(cpu, 0xfffd) << 8;
    cpu->PC = addr;
}

void CPU_irq(CPU_t* cpu)
{
    if(!cpu) return;
    // if interrupt flag disable return
    if(getFlag(cpu, 'i') == true){
        return;
    }

    // pull irq handler addres from the interrupt vector 
    word addr;
    addr = CPU_read(cpu, 0xfffe);
    addr += CPU_read(cpu, 0xffff) << 8;

    // push return addres to stack
    push(cpu, (byte)(cpu->PC >> 8));
    push(cpu, (byte)(cpu->PC >> 0));

    // push P(flags on the stack)
    setFlag(cpu, 'b', false);
    push(cpu, cpu->P);

    // jump to handler
    cpu->PC = addr;

    setFlag(cpu, 'i', true);
}

void CPU_nmi(CPU_t* cpu)
{
    if(!cpu) return;
    // if interrupt flag disable return
    if(getFlag(cpu, 'i') == true){
        return;
    }

    // pull irq handler addres from the interrupt vector 
    word addr;
    addr = CPU_read(cpu, 0xfffa);
    addr += CPU_read(cpu, 0xfffb) << 8;

    // push return addres to stack
    push(cpu, (byte)(cpu->PC >> 8));
    push(cpu, (byte)(cpu->PC >> 0));

    // push P(flags on the stack)
    setFlag(cpu, 'b', false);
    push(cpu, cpu->P);

    // jump to handler
    cpu->PC = addr;

    setFlag(cpu, 'i', true);
}
