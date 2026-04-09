
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

typedef void (*Instruction_t)(CPU_t*);
Instruction_t Opcode_to_Instruction_table[0xff + 1];

static void  CPU_setFlag(CPU_t* cpu, char flag, bool value)
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

static bool  CPU_getFlag(CPU_t* cpu, char flag){
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

static void  CPU_update_NZCV_add(CPU_t* cpu, byte a, byte b, byte r){
    if(!cpu) return;
    CPU_setFlag(cpu, 'z', !r);
	CPU_setFlag(cpu, 'n', r >> 7);
	CPU_setFlag(cpu, 'c', ((word)a + b) >> 8);
	CPU_setFlag(cpu, 'v', (~(a ^ b) & (a ^ r)) >> 7);
}

static void  CPU_update_NZCV_sub(CPU_t* cpu, byte a, byte b, byte r){
    if(!cpu) return;
    CPU_setFlag(cpu, 'z', r == 0);
	CPU_setFlag(cpu, 'n', r >> 7);
	CPU_setFlag(cpu, 'c', a >= b);
	CPU_setFlag(cpu, 'v', ((a ^ b) & (a ^ r)) >> 7);
}

static void  CPU_update_NZ(CPU_t* cpu, byte r){
    if(!cpu) return;
    CPU_setFlag(cpu, 'z', !r);
	CPU_setFlag(cpu, 'n', r >> 7);
}

// TODO: fix bugs in zero page mods
static void CPU_helper_load(CPU_t* cpu, char reg, Addressing_mode_t amod){
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
        CPU_update_NZ(cpu, cpu->A);
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
        CPU_update_NZ(cpu, cpu->X);
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
        CPU_update_NZ(cpu, cpu->Y);
    }
    else{
        CPU_ERROR(cpu, "invalid register: \"%c\" for load operation", reg);
    }
}

// TODO: fix bugs in zero page mods
static void CPU_helper_store(CPU_t* cpu, char reg, Addressing_mode_t amod){
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

static void CPU_helper_and(CPU_t* cpu, Addressing_mode_t amod){
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
    CPU_update_NZ(cpu, cpu->A);
}

static void CPU_helper_eor(CPU_t* cpu, Addressing_mode_t amod){
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
    CPU_update_NZ(cpu, cpu->A);
}

static void CPU_helper_ora(CPU_t* cpu, Addressing_mode_t amod){
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
    CPU_update_NZ(cpu, cpu->A);
}

static void CPU_push(CPU_t* cpu, byte value){
    if(!cpu) return;
    CPU_write(cpu, 0x0100 + --cpu->SP, value);
}

static byte CPU_pop(CPU_t* cpu){
    if(!cpu) return 0xff;
    return CPU_read(cpu, 0x0100 + cpu->SP++);
}

// ===================================================================================
// user interface:
// ===================================================================================

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

void CPU_reset(CPU_t* cpu)
{
    if(!cpu) return;
    cpu->SP = 0xfd;
    CPU_setFlag(cpu, 'u', true);
    CPU_setFlag(cpu, 'i', true);
    CPU_setFlag(cpu, 'd', false);

    word addr;
    addr = CPU_read(cpu, 0xfffc);
    addr += CPU_read(cpu, 0xfffd) << 8;
    cpu->PC = addr;
}

void CPU_irq(CPU_t* cpu)
{
    if(!cpu) return;
    // if interrupt flag disable return
    if(CPU_getFlag(cpu, 'i') == true){
        return;
    }

    // pull irq handler addres from the interrupt vector 
    word addr;
    addr = CPU_read(cpu, 0xfffe);
    addr += CPU_read(cpu, 0xffff) << 8;

    // CPU_push return addres to stack
    CPU_push(cpu, (byte)(cpu->PC >> 8));
    CPU_push(cpu, (byte)(cpu->PC >> 0));

    // CPU_push P(flags on the stack)
    CPU_setFlag(cpu, 'b', false);
    CPU_push(cpu, cpu->P);

    // jump to handler
    cpu->PC = addr;

    CPU_setFlag(cpu, 'i', true);
}

void CPU_nmi(CPU_t* cpu)
{
    if(!cpu) return;
    // if interrupt flag disable return
    if(CPU_getFlag(cpu, 'i') == true){
        return;
    }

    // pull irq handler addres from the interrupt vector 
    word addr;
    addr = CPU_read(cpu, 0xfffa);
    addr += CPU_read(cpu, 0xfffb) << 8;

    // CPU_push return addres to stack
    CPU_push(cpu, (byte)(cpu->PC >> 8));
    CPU_push(cpu, (byte)(cpu->PC >> 0));

    // CPU_push P(flags on the stack)
    CPU_setFlag(cpu, 'b', false);
    CPU_push(cpu, cpu->P);

    // jump to handler
    cpu->PC = addr;

    CPU_setFlag(cpu, 'i', true);
}

// ===================================================================================
// instructions:
// ===================================================================================

// Load/Store
static void instruction_LDA_Immediate(CPU_t* CPU_t);
static void instruction_LDA_ZeroPage(CPU_t* CPU_t);
static void instruction_LDA_ZeroPageX(CPU_t* CPU_t);
static void instruction_LDA_Absolute(CPU_t* CPU_t);
static void instruction_LDA_AbsoluteX(CPU_t* CPU_t);
static void instruction_LDA_AbsoluteY(CPU_t* CPU_t);
static void instruction_LDA_IndirectX(CPU_t* CPU_t);
static void instruction_LDA_IndirectY(CPU_t* CPU_t);

static void instruction_LDX_Immediate(CPU_t* CPU_t);
static void instruction_LDX_ZeroPage(CPU_t* CPU_t);
static void instruction_LDX_ZeroPageY(CPU_t* CPU_t);
static void instruction_LDX_Absolute(CPU_t* CPU_t);
static void instruction_LDX_AbsoluteY(CPU_t* CPU_t);

static void instruction_LDY_Immediate(CPU_t* CPU_t);
static void instruction_LDY_ZeroPage(CPU_t* CPU_t);
static void instruction_LDY_ZeroPageX(CPU_t* CPU_t);
static void instruction_LDY_Absolute(CPU_t* CPU_t);
static void instruction_LDY_AbsoluteX(CPU_t* CPU_t);

static void instruction_STA_ZeroPage(CPU_t* CPU_t);
static void instruction_STA_ZeroPageX(CPU_t* CPU_t);
static void instruction_STA_Absolute(CPU_t* CPU_t);
static void instruction_STA_AbsoluteX(CPU_t* CPU_t);
static void instruction_STA_AbsoluteY(CPU_t* CPU_t);
static void instruction_STA_IndirectX(CPU_t* CPU_t);
static void instruction_STA_IndirectY(CPU_t* CPU_t);

static void instruction_STX_ZeroPage(CPU_t* CPU_t);
static void instruction_STX_ZeroPageY(CPU_t* CPU_t);
static void instruction_STX_Absolute(CPU_t* CPU_t);

static void instruction_STY_ZeroPage(CPU_t* CPU_t);
static void instruction_STY_ZeroPageX(CPU_t* CPU_t);
static void instruction_STY_Absolute(CPU_t* CPU_t);

// Register Transfers
static void instruction_TAX(CPU_t* CPU_t);
static void instruction_TAY(CPU_t* CPU_t);
static void instruction_TSX(CPU_t* CPU_t);
static void instruction_TXA(CPU_t* CPU_t);
static void instruction_TXS(CPU_t* CPU_t);
static void instruction_TYA(CPU_t* CPU_t);

// Stack
static void instruction_PHA(CPU_t* CPU_t);
static void instruction_PHP(CPU_t* CPU_t);
static void instruction_PLA(CPU_t* CPU_t);
static void instruction_PLP(CPU_t* CPU_t);

// Logical
static void instruction_AND_Immediate(CPU_t* CPU_t);
static void instruction_AND_ZeroPage(CPU_t* CPU_t);
static void instruction_AND_ZeroPageX(CPU_t* CPU_t);
static void instruction_AND_Absolute(CPU_t* CPU_t);
static void instruction_AND_AbsoluteX(CPU_t* CPU_t);
static void instruction_AND_AbsoluteY(CPU_t* CPU_t);
static void instruction_AND_IndirectX(CPU_t* CPU_t);
static void instruction_AND_IndirectY(CPU_t* CPU_t);

static void instruction_EOR_Immediate(CPU_t* CPU_t);
static void instruction_EOR_ZeroPage(CPU_t* CPU_t);
static void instruction_EOR_ZeroPageX(CPU_t* CPU_t);
static void instruction_EOR_Absolute(CPU_t* CPU_t);
static void instruction_EOR_AbsoluteX(CPU_t* CPU_t);
static void instruction_EOR_AbsoluteY(CPU_t* CPU_t);
static void instruction_EOR_IndirectX(CPU_t* CPU_t);
static void instruction_EOR_IndirectY(CPU_t* CPU_t);

static void instruction_ORA_Immediate(CPU_t* CPU_t);
static void instruction_ORA_ZeroPage(CPU_t* CPU_t);
static void instruction_ORA_ZeroPageX(CPU_t* CPU_t);
static void instruction_ORA_Absolute(CPU_t* CPU_t);
static void instruction_ORA_AbsoluteX(CPU_t* CPU_t);
static void instruction_ORA_AbsoluteY(CPU_t* CPU_t);
static void instruction_ORA_IndirectX(CPU_t* CPU_t);
static void instruction_ORA_IndirectY(CPU_t* CPU_t);

static void instruction_BIT_ZeroPage(CPU_t* CPU_t);
static void instruction_BIT_Absolute(CPU_t* CPU_t);

// Arithmetic
static void instruction_ADC_Immediate(CPU_t* CPU_t);
static void instruction_ADC_ZeroPage(CPU_t* CPU_t);
static void instruction_ADC_ZeroPageX(CPU_t* CPU_t);
static void instruction_ADC_Absolute(CPU_t* CPU_t);
static void instruction_ADC_AbsoluteX(CPU_t* CPU_t);
static void instruction_ADC_AbsoluteY(CPU_t* CPU_t);
static void instruction_ADC_IndirectX(CPU_t* CPU_t);
static void instruction_ADC_IndirectY(CPU_t* CPU_t);

static void instruction_SBC_Immediate(CPU_t* CPU_t);
static void instruction_SBC_ZeroPage(CPU_t* CPU_t);
static void instruction_SBC_ZeroPageX(CPU_t* CPU_t);
static void instruction_SBC_Absolute(CPU_t* CPU_t);
static void instruction_SBC_AbsoluteX(CPU_t* CPU_t);
static void instruction_SBC_AbsoluteY(CPU_t* CPU_t);
static void instruction_SBC_IndirectX(CPU_t* CPU_t);
static void instruction_SBC_IndirectY(CPU_t* CPU_t);

static void instruction_CMP_Immediate(CPU_t* CPU_t);
static void instruction_CMP_ZeroPage(CPU_t* CPU_t);
static void instruction_CMP_ZeroPageX(CPU_t* CPU_t);
static void instruction_CMP_Absolute(CPU_t* CPU_t);
static void instruction_CMP_AbsoluteX(CPU_t* CPU_t);
static void instruction_CMP_AbsoluteY(CPU_t* CPU_t);
static void instruction_CMP_IndirectX(CPU_t* CPU_t);
static void instruction_CMP_IndirectY(CPU_t* CPU_t);

static void instruction_CPX_Immediate(CPU_t* CPU_t);
static void instruction_CPX_ZeroPage(CPU_t* CPU_t);
static void instruction_CPX_Absolute(CPU_t* CPU_t);

static void instruction_CPY_Immediate(CPU_t* CPU_t);
static void instruction_CPY_ZeroPage(CPU_t* CPU_t);
static void instruction_CPY_Absolute(CPU_t* CPU_t);

// Increments / Decrements
static void instruction_INC_ZeroPage(CPU_t* CPU_t);
static void instruction_INC_ZeroPageX(CPU_t* CPU_t);
static void instruction_INC_Absolute(CPU_t* CPU_t);
static void instruction_INC_AbsoluteX(CPU_t* CPU_t);

static void instruction_INX(CPU_t* CPU_t);
static void instruction_INY(CPU_t* CPU_t);

static void instruction_DEC_ZeroPage(CPU_t* CPU_t);
static void instruction_DEC_ZeroPageX(CPU_t* CPU_t);
static void instruction_DEC_Absolute(CPU_t* CPU_t);
static void instruction_DEC_AbsoluteX(CPU_t* CPU_t);

static void instruction_DEX(CPU_t* CPU_t);
static void instruction_DEY(CPU_t* CPU_t);

// Shifts / Rotates
static void instruction_ASL_Accumulator(CPU_t* CPU_t);
static void instruction_ASL_ZeroPage(CPU_t* CPU_t);
static void instruction_ASL_ZeroPageX(CPU_t* CPU_t);
static void instruction_ASL_Absolute(CPU_t* CPU_t);
static void instruction_ASL_AbsoluteX(CPU_t* CPU_t);

static void instruction_LSR_Accumulator(CPU_t* CPU_t);
static void instruction_LSR_ZeroPage(CPU_t* CPU_t);
static void instruction_LSR_ZeroPageX(CPU_t* CPU_t);
static void instruction_LSR_Absolute(CPU_t* CPU_t);
static void instruction_LSR_AbsoluteX(CPU_t* CPU_t);

static void instruction_ROL_Accumulator(CPU_t* CPU_t);
static void instruction_ROL_ZeroPage(CPU_t* CPU_t);
static void instruction_ROL_ZeroPageX(CPU_t* CPU_t);
static void instruction_ROL_Absolute(CPU_t* CPU_t);
static void instruction_ROL_AbsoluteX(CPU_t* CPU_t);

static void instruction_ROR_Accumulator(CPU_t* CPU_t);
static void instruction_ROR_ZeroPage(CPU_t* CPU_t);
static void instruction_ROR_ZeroPageX(CPU_t* CPU_t);
static void instruction_ROR_Absolute(CPU_t* CPU_t);
static void instruction_ROR_AbsoluteX(CPU_t* CPU_t);

// Jumps / Calls
static void instruction_JMP_Absolute(CPU_t* CPU_t);
static void instruction_JMP_Indirect(CPU_t* CPU_t);
static void instruction_JSR(CPU_t* CPU_t);
static void instruction_RTS(CPU_t* CPU_t);

// Branches
static void instruction_BCC(CPU_t* CPU_t);
static void instruction_BCS(CPU_t* CPU_t);
static void instruction_BEQ(CPU_t* CPU_t);
static void instruction_BMI(CPU_t* CPU_t);
static void instruction_BNE(CPU_t* CPU_t);
static void instruction_BPL(CPU_t* CPU_t);
static void instruction_BVC(CPU_t* CPU_t);
static void instruction_BVS(CPU_t* CPU_t);

// Flags
static void instruction_CLC(CPU_t* CPU_t);
static void instruction_CLD(CPU_t* CPU_t);
static void instruction_CLI(CPU_t* CPU_t);
static void instruction_CLV(CPU_t* CPU_t);
static void instruction_SEC(CPU_t* CPU_t);
static void instruction_SED(CPU_t* CPU_t);
static void instruction_SEI(CPU_t* CPU_t);

// System
static void instruction_BRK(CPU_t* CPU_t);
static void instruction_NOP(CPU_t* CPU_t);
static void instruction_RTI(CPU_t* CPU_t);

Instruction_t Opcode_to_Instruction_table[0xff + 1] = {
    // Load/Store
    [Opcode_LDA_Immediate]   = instruction_LDA_Immediate,
    [Opcode_LDA_ZeroPage]    = instruction_LDA_ZeroPage,
    [Opcode_LDA_ZeroPageX]   = instruction_LDA_ZeroPageX,
    [Opcode_LDA_Absolute]    = instruction_LDA_Absolute,
    [Opcode_LDA_AbsoluteX]   = instruction_LDA_AbsoluteX,
    [Opcode_LDA_AbsoluteY]   = instruction_LDA_AbsoluteY,
    [Opcode_LDA_IndirectX]   = instruction_LDA_IndirectX,
    [Opcode_LDA_IndirectY]   = instruction_LDA_IndirectY,

    [Opcode_LDX_Immediate]   = instruction_LDX_Immediate,
    [Opcode_LDX_ZeroPage]    = instruction_LDX_ZeroPage,
    [Opcode_LDX_ZeroPageY]   = instruction_LDX_ZeroPageY,
    [Opcode_LDX_Absolute]    = instruction_LDX_Absolute,
    [Opcode_LDX_AbsoluteY]   = instruction_LDX_AbsoluteY,

    [Opcode_LDY_Immediate]   = instruction_LDY_Immediate,
    [Opcode_LDY_ZeroPage]    = instruction_LDY_ZeroPage,
    [Opcode_LDY_ZeroPageX]   = instruction_LDY_ZeroPageX,
    [Opcode_LDY_Absolute]    = instruction_LDY_Absolute,
    [Opcode_LDY_AbsoluteX]   = instruction_LDY_AbsoluteX,

    [Opcode_STA_ZeroPage]    = instruction_STA_ZeroPage,
    [Opcode_STA_ZeroPageX]   = instruction_STA_ZeroPageX,
    [Opcode_STA_Absolute]    = instruction_STA_Absolute,
    [Opcode_STA_AbsoluteX]   = instruction_STA_AbsoluteX,
    [Opcode_STA_AbsoluteY]   = instruction_STA_AbsoluteY,
    [Opcode_STA_IndirectX]   = instruction_STA_IndirectX,
    [Opcode_STA_IndirectY]   = instruction_STA_IndirectY,

    [Opcode_STX_ZeroPage]    = instruction_STX_ZeroPage,
    [Opcode_STX_ZeroPageY]   = instruction_STX_ZeroPageY,
    [Opcode_STX_Absolute]    = instruction_STX_Absolute,

    [Opcode_STY_ZeroPage]    = instruction_STY_ZeroPage,
    [Opcode_STY_ZeroPageX]   = instruction_STY_ZeroPageX,
    [Opcode_STY_Absolute]    = instruction_STY_Absolute,

    // Register Transfers
    [Opcode_TAX]             = instruction_TAX,
    [Opcode_TAY]             = instruction_TAY,
    [Opcode_TSX]             = instruction_TSX,
    [Opcode_TXA]             = instruction_TXA,
    [Opcode_TXS]             = instruction_TXS,
    [Opcode_TYA]             = instruction_TYA,

    // // Stack
    [Opcode_PHA]             = instruction_PHA,
    [Opcode_PHP]             = instruction_PHP,
    [Opcode_PLA]             = instruction_PLA,
    [Opcode_PLP]             = instruction_PLP,

    // Logical
    [Opcode_AND_Immediate]   = instruction_AND_Immediate,
    [Opcode_AND_ZeroPage]    = instruction_AND_ZeroPage,
    [Opcode_AND_ZeroPageX]   = instruction_AND_ZeroPageX,
    [Opcode_AND_Absolute]    = instruction_AND_Absolute,
    [Opcode_AND_AbsoluteX]   = instruction_AND_AbsoluteX,
    [Opcode_AND_AbsoluteY]   = instruction_AND_AbsoluteY,
    [Opcode_AND_IndirectX]   = instruction_AND_IndirectX,
    [Opcode_AND_IndirectY]   = instruction_AND_IndirectY,

    [Opcode_EOR_Immediate]   = instruction_EOR_Immediate,
    [Opcode_EOR_ZeroPage]    = instruction_EOR_ZeroPage,
    [Opcode_EOR_ZeroPageX]   = instruction_EOR_ZeroPageX,
    [Opcode_EOR_Absolute]    = instruction_EOR_Absolute,
    [Opcode_EOR_AbsoluteX]   = instruction_EOR_AbsoluteX,
    [Opcode_EOR_AbsoluteY]   = instruction_EOR_AbsoluteY,
    [Opcode_EOR_IndirectX]   = instruction_EOR_IndirectX,
    [Opcode_EOR_IndirectY]   = instruction_EOR_IndirectY,

    [Opcode_ORA_Immediate]   = instruction_ORA_Immediate,
    [Opcode_ORA_ZeroPage]    = instruction_ORA_ZeroPage,
    [Opcode_ORA_ZeroPageX]   = instruction_ORA_ZeroPageX,
    [Opcode_ORA_Absolute]    = instruction_ORA_Absolute,
    [Opcode_ORA_AbsoluteX]   = instruction_ORA_AbsoluteX,
    [Opcode_ORA_AbsoluteY]   = instruction_ORA_AbsoluteY,
    [Opcode_ORA_IndirectX]   = instruction_ORA_IndirectX,
    [Opcode_ORA_IndirectY]   = instruction_ORA_IndirectY,

    [Opcode_BIT_ZeroPage]    = instruction_BIT_ZeroPage,
    [Opcode_BIT_Absolute]    = instruction_BIT_Absolute,

    // Arithmetic
    [Opcode_ADC_Immediate]   = instruction_ADC_Immediate,
    [Opcode_ADC_ZeroPage]    = instruction_ADC_ZeroPage,
    [Opcode_ADC_ZeroPageX]   = instruction_ADC_ZeroPageX,
    [Opcode_ADC_Absolute]    = instruction_ADC_Absolute,
    [Opcode_ADC_AbsoluteX]   = instruction_ADC_AbsoluteX,
    [Opcode_ADC_AbsoluteY]   = instruction_ADC_AbsoluteY,
    [Opcode_ADC_IndirectX]   = instruction_ADC_IndirectX,
    [Opcode_ADC_IndirectY]   = instruction_ADC_IndirectY,

    [Opcode_SBC_Immediate]   = instruction_SBC_Immediate,
    [Opcode_SBC_ZeroPage]    = instruction_SBC_ZeroPage,
    [Opcode_SBC_ZeroPageX]   = instruction_SBC_ZeroPageX,
    [Opcode_SBC_Absolute]    = instruction_SBC_Absolute,
    [Opcode_SBC_AbsoluteX]   = instruction_SBC_AbsoluteX,
    [Opcode_SBC_AbsoluteY]   = instruction_SBC_AbsoluteY,
    [Opcode_SBC_IndirectX]   = instruction_SBC_IndirectX,
    [Opcode_SBC_IndirectY]   = instruction_SBC_IndirectY,

    [Opcode_CMP_Immediate]   = instruction_CMP_Immediate,
    [Opcode_CMP_ZeroPage]    = instruction_CMP_ZeroPage,
    [Opcode_CMP_ZeroPageX]   = instruction_CMP_ZeroPageX,
    [Opcode_CMP_Absolute]    = instruction_CMP_Absolute,
    [Opcode_CMP_AbsoluteX]   = instruction_CMP_AbsoluteX,
    [Opcode_CMP_AbsoluteY]   = instruction_CMP_AbsoluteY,
    [Opcode_CMP_IndirectX]   = instruction_CMP_IndirectX,
    [Opcode_CMP_IndirectY]   = instruction_CMP_IndirectY,

    [Opcode_CPX_Immediate]   = instruction_CPX_Immediate,
    [Opcode_CPX_ZeroPage]    = instruction_CPX_ZeroPage,
    [Opcode_CPX_Absolute]    = instruction_CPX_Absolute,

    [Opcode_CPY_Immediate]   = instruction_CPY_Immediate,
    [Opcode_CPY_ZeroPage]    = instruction_CPY_ZeroPage,
    [Opcode_CPY_Absolute]    = instruction_CPY_Absolute,

    // Increments / Decrements
    [Opcode_INC_ZeroPage]    = instruction_INC_ZeroPage,
    [Opcode_INC_ZeroPageX]   = instruction_INC_ZeroPageX,
    [Opcode_INC_Absolute]    = instruction_INC_Absolute,
    [Opcode_INC_AbsoluteX]   = instruction_INC_AbsoluteX,

    [Opcode_INX]             = instruction_INX,
    [Opcode_INY]             = instruction_INY,

    [Opcode_DEC_ZeroPage]    = instruction_DEC_ZeroPage,
    [Opcode_DEC_ZeroPageX]   = instruction_DEC_ZeroPageX,
    [Opcode_DEC_Absolute]    = instruction_DEC_Absolute,
    [Opcode_DEC_AbsoluteX]   = instruction_DEC_AbsoluteX,

    [Opcode_DEX]             = instruction_DEX,
    [Opcode_DEY]             = instruction_DEY,

    // Shifts / Rotates
    [Opcode_ASL_Accumulator] = instruction_ASL_Accumulator,
    [Opcode_ASL_ZeroPage]    = instruction_ASL_ZeroPage,
    [Opcode_ASL_ZeroPageX]   = instruction_ASL_ZeroPageX,
    [Opcode_ASL_Absolute]    = instruction_ASL_Absolute,
    [Opcode_ASL_AbsoluteX]   = instruction_ASL_AbsoluteX,

    [Opcode_LSR_Accumulator] = instruction_LSR_Accumulator,
    [Opcode_LSR_ZeroPage]    = instruction_LSR_ZeroPage,
    [Opcode_LSR_ZeroPageX]   = instruction_LSR_ZeroPageX,
    [Opcode_LSR_Absolute]    = instruction_LSR_Absolute,
    [Opcode_LSR_AbsoluteX]   = instruction_LSR_AbsoluteX,

    [Opcode_ROL_Accumulator] = instruction_ROL_Accumulator,
    [Opcode_ROL_ZeroPage]    = instruction_ROL_ZeroPage,
    [Opcode_ROL_ZeroPageX]   = instruction_ROL_ZeroPageX,
    [Opcode_ROL_Absolute]    = instruction_ROL_Absolute,
    [Opcode_ROL_AbsoluteX]   = instruction_ROL_AbsoluteX,

    [Opcode_ROR_Accumulator] = instruction_ROR_Accumulator,
    [Opcode_ROR_ZeroPage]    = instruction_ROR_ZeroPage,
    [Opcode_ROR_ZeroPageX]   = instruction_ROR_ZeroPageX,
    [Opcode_ROR_Absolute]    = instruction_ROR_Absolute,
    [Opcode_ROR_AbsoluteX]   = instruction_ROR_AbsoluteX,

    // Jumps & Calls
    [Opcode_JMP_Absolute]    = instruction_JMP_Absolute,
    [Opcode_JMP_Indirect]    = instruction_JMP_Indirect,
    [Opcode_JSR]             = instruction_JSR,
    [Opcode_RTS]             = instruction_RTS,

    // Branches
    [Opcode_BCC]             = instruction_BCC,
    [Opcode_BCS]             = instruction_BCS,
    [Opcode_BEQ]             = instruction_BEQ,
    [Opcode_BMI]             = instruction_BMI,
    [Opcode_BNE]             = instruction_BNE,
    [Opcode_BPL]             = instruction_BPL,
    [Opcode_BVC]             = instruction_BVC,
    [Opcode_BVS]             = instruction_BVS,

    // Flags
    [Opcode_CLC]             = instruction_CLC,
    [Opcode_CLD]             = instruction_CLD,
    [Opcode_CLI]             = instruction_CLI,
    [Opcode_CLV]             = instruction_CLV,
    [Opcode_SEC]             = instruction_SEC,
    [Opcode_SED]             = instruction_SED,
    [Opcode_SEI]             = instruction_SEI,

    // System
    [Opcode_BRK]             = instruction_BRK,
    [Opcode_NOP]             = instruction_NOP,
    [Opcode_RTI]             = instruction_RTI,

};

static void instruction_LDA_Immediate(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_Immediate);
}
static void instruction_LDA_ZeroPage(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_ZeroPage);

}
static void instruction_LDA_ZeroPageX(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_ZeroPageX);

}
static void instruction_LDA_Absolute(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_Absolute);

}
static void instruction_LDA_AbsoluteX(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_AbsoluteX);

}
static void instruction_LDA_AbsoluteY(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_AbsoluteY);

}
static void instruction_LDA_IndirectX(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_IndirectX);

}
static void instruction_LDA_IndirectY(CPU_t* cpu) 
{
    CPU_helper_load(cpu, 'A', Addressing_mode_IndirectY);

}

static void instruction_LDX_Immediate(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'X', Addressing_mode_Immediate);

}
static void instruction_LDX_ZeroPage(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'X', Addressing_mode_ZeroPage);

}
static void instruction_LDX_ZeroPageY(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'X', Addressing_mode_ZeroPageY);

}
static void instruction_LDX_Absolute(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'X', Addressing_mode_Absolute);

}
static void instruction_LDX_AbsoluteY(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'X', Addressing_mode_AbsoluteY);

}

static void instruction_LDY_Immediate(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'Y', Addressing_mode_Immediate);

}
static void instruction_LDY_ZeroPage(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'Y', Addressing_mode_ZeroPage);

}
static void instruction_LDY_ZeroPageX(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'Y', Addressing_mode_ZeroPageX);

}
static void instruction_LDY_Absolute(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'Y', Addressing_mode_Absolute);

}
static void instruction_LDY_AbsoluteX(CPU_t* cpu)
{
    CPU_helper_load(cpu, 'Y', Addressing_mode_Absolute);

}

static void instruction_STA_ZeroPage(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_ZeroPage);
}
static void instruction_STA_ZeroPageX(CPU_t* cpu) 
{
    CPU_helper_store(cpu, 'A', Addressing_mode_ZeroPageX);
}
static void instruction_STA_Absolute(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_Absolute);

}
static void instruction_STA_AbsoluteX(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_AbsoluteX);

}
static void instruction_STA_AbsoluteY(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_AbsoluteY);

}
static void instruction_STA_IndirectX(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_IndirectX);

}
static void instruction_STA_IndirectY(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'A', Addressing_mode_IndirectY);

}

static void instruction_STX_ZeroPage(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'X', Addressing_mode_ZeroPage);

}
static void instruction_STX_ZeroPageY(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'X', Addressing_mode_ZeroPageY);

}
static void instruction_STX_Absolute(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'X', Addressing_mode_Absolute);

}

static void instruction_STY_ZeroPage(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'Y', Addressing_mode_ZeroPage);

}
static void instruction_STY_ZeroPageX(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'Y', Addressing_mode_ZeroPageX);

}
static void instruction_STY_Absolute(CPU_t* cpu)
{
    CPU_helper_store(cpu, 'Y', Addressing_mode_Absolute);
}

// Register Transfers
void instruction_TAX(CPU_t* cpu)
{
    cpu->X = cpu->A;
    CPU_update_NZ(cpu, cpu->X);
}
void instruction_TAY(CPU_t* cpu)
{
    cpu->Y = cpu->A;
    CPU_update_NZ(cpu, cpu->Y);
}
void instruction_TSX(CPU_t* cpu)
{
    cpu->X = cpu->SP;
    CPU_update_NZ(cpu, cpu->X);
}
void instruction_TXA(CPU_t* cpu)
{
    cpu->A = cpu->X;
    CPU_update_NZ(cpu, cpu->A);
}
void instruction_TXS(CPU_t* cpu)
{
    cpu->SP = cpu->X;
    // not modify the flags register 
}
void instruction_TYA(CPU_t* cpu)
{
    cpu->A = cpu->Y;
    CPU_update_NZ(cpu, cpu->A);
}

// Stack
void instruction_PHA(CPU_t* cpu)
{
    CPU_push(cpu, cpu->A);
}
void instruction_PHP(CPU_t* cpu)
{
    CPU_push(cpu, cpu->P);
}
void instruction_PLA(CPU_t* cpu)
{
    cpu->A = CPU_pop(cpu);
    CPU_update_NZ(cpu, cpu->A);
}
void instruction_PLP(CPU_t* cpu)
{
    cpu->P = CPU_pop(cpu);
}

// Logical
static void instruction_AND_Immediate(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_Immediate);

}
static void instruction_AND_ZeroPage(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_ZeroPage);

}
static void instruction_AND_ZeroPageX(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_ZeroPageX);

}
static void instruction_AND_Absolute(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_Absolute);

}
static void instruction_AND_AbsoluteX(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_AbsoluteX);

}
static void instruction_AND_AbsoluteY(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_AbsoluteY);

}
static void instruction_AND_IndirectX(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_IndirectX);

}
static void instruction_AND_IndirectY(CPU_t* cpu)
{
    CPU_helper_and(cpu, Addressing_mode_IndirectY);
}

static void instruction_EOR_Immediate(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_Immediate);
}
static void instruction_EOR_ZeroPage(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_ZeroPage);
}
static void instruction_EOR_ZeroPageX(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_ZeroPageX);
}
static void instruction_EOR_Absolute(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_Absolute);
}
static void instruction_EOR_AbsoluteX(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_AbsoluteX);
}
static void instruction_EOR_AbsoluteY(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_AbsoluteY);
}
static void instruction_EOR_IndirectX(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_IndirectX);
}
static void instruction_EOR_IndirectY(CPU_t* cpu)
{
    CPU_helper_eor(cpu, Addressing_mode_IndirectY);
}

static void instruction_ORA_Immediate(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_Immediate);
}
static void instruction_ORA_ZeroPage(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_ZeroPage);
}
static void instruction_ORA_ZeroPageX(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_ZeroPageX);
}
static void instruction_ORA_Absolute(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_Absolute);
}
static void instruction_ORA_AbsoluteX(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_AbsoluteX);
}
static void instruction_ORA_AbsoluteY(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_AbsoluteY);
}
static void instruction_ORA_IndirectX(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_IndirectX);
}
static void instruction_ORA_IndirectY(CPU_t* cpu)
{
    CPU_helper_ora(cpu, Addressing_mode_IndirectY);
}

static void instruction_BIT_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_BIT_Absolute(CPU_t* cpu) { UNUSED(); }

// Arithmetic
static void instruction_ADC_Immediate(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_AbsoluteX(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_AbsoluteY(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_IndirectX(CPU_t* cpu) { UNUSED(); }
static void instruction_ADC_IndirectY(CPU_t* cpu) { UNUSED(); }

static void instruction_SBC_Immediate(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_AbsoluteX(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_AbsoluteY(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_IndirectX(CPU_t* cpu) { UNUSED(); }
static void instruction_SBC_IndirectY(CPU_t* cpu) { UNUSED(); }
 
static void instruction_CMP_Immediate(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_ZeroPage(CPU_t* cpu) { UNUSED(); }

static void instruction_CMP_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_AbsoluteX(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_AbsoluteY(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_IndirectX(CPU_t* cpu) { UNUSED(); }
static void instruction_CMP_IndirectY(CPU_t* cpu) { UNUSED(); }

static void instruction_CPX_Immediate(CPU_t* cpu) { UNUSED(); }
static void instruction_CPX_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_CPX_Absolute(CPU_t* cpu) { UNUSED(); }

static void instruction_CPY_Immediate(CPU_t* cpu) { UNUSED(); }
static void instruction_CPY_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_CPY_Absolute(CPU_t* cpu) { UNUSED(); }


// Increments / Decrements
static void instruction_INC_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_INC_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_INC_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_INC_AbsoluteX(CPU_t* cpu) { UNUSED(); }

static void instruction_INX(CPU_t* cpu) { UNUSED(); }
static void instruction_INY(CPU_t* cpu) { UNUSED(); }

static void instruction_DEC_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_DEC_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_DEC_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_DEC_AbsoluteX(CPU_t* cpu) { UNUSED(); }

static void instruction_DEX(CPU_t* cpu) { UNUSED(); }
static void instruction_DEY(CPU_t* cpu) { UNUSED(); }

// Shifts / Rotates
static void instruction_ASL_Accumulator(CPU_t* cpu) { UNUSED(); }
static void instruction_ASL_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_ASL_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_ASL_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_ASL_AbsoluteX(CPU_t* cpu) { UNUSED(); }

static void instruction_LSR_Accumulator(CPU_t* cpu) { UNUSED(); }
static void instruction_LSR_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_LSR_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_LSR_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_LSR_AbsoluteX(CPU_t* cpu) { UNUSED(); }

static void instruction_ROL_Accumulator(CPU_t* cpu) { UNUSED(); }
static void instruction_ROL_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_ROL_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_ROL_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_ROL_AbsoluteX(CPU_t* cpu) { UNUSED(); }

static void instruction_ROR_Accumulator(CPU_t* cpu) { UNUSED(); }
static void instruction_ROR_ZeroPage(CPU_t* cpu) { UNUSED(); }
static void instruction_ROR_ZeroPageX(CPU_t* cpu) { UNUSED(); }
static void instruction_ROR_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_ROR_AbsoluteX(CPU_t* cpu) { UNUSED(); }

// Jumps / Calls
static void instruction_JMP_Absolute(CPU_t* cpu) { UNUSED(); }
static void instruction_JMP_Indirect(CPU_t* cpu) { UNUSED(); }
static void instruction_JSR(CPU_t* cpu) { UNUSED(); }
static void instruction_RTS(CPU_t* cpu) { UNUSED(); }

// Branches
static void instruction_BCC(CPU_t* cpu) { UNUSED(); }
static void instruction_BCS(CPU_t* cpu) { UNUSED(); }
static void instruction_BEQ(CPU_t* cpu) { UNUSED(); }
static void instruction_BMI(CPU_t* cpu) { UNUSED(); }
static void instruction_BNE(CPU_t* cpu) { UNUSED(); }
static void instruction_BPL(CPU_t* cpu) { UNUSED(); }
static void instruction_BVC(CPU_t* cpu) { UNUSED(); }
static void instruction_BVS(CPU_t* cpu) { UNUSED(); }

// Flags
static void instruction_CLC(CPU_t* cpu) { UNUSED(); }
static void instruction_CLD(CPU_t* cpu) { UNUSED(); }
static void instruction_CLI(CPU_t* cpu) { UNUSED(); }
static void instruction_CLV(CPU_t* cpu) { UNUSED(); }
static void instruction_SEC(CPU_t* cpu) { UNUSED(); }
static void instruction_SED(CPU_t* cpu) { UNUSED(); }
static void instruction_SEI(CPU_t* cpu) { UNUSED(); }

// System
static void instruction_BRK(CPU_t* cpu) { UNUSED(); }
static void instruction_NOP(CPU_t* cpu) { UNUSED(); }
static void instruction_RTI(CPU_t* cpu) { UNUSED(); }
