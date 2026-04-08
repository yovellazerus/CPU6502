
#include "cpu.h"

typedef struct {
    FNdestroy destroy;
    FNread read;
    FNwrite write;
    FNdump dump;
} Bus_t;

struct CPU_t
{
    byte A;
    byte X;
    byte Y;
    byte SP;
    byte P;
    word PC;

    // memory read/write abstraction
    Bus_t* bus;
};

static void CPU_push(CPU_t* cpu, byte value){
    if(!cpu) return;
    CPU_write(cpu, 0x0100 + --cpu->SP, value);
}

static byte CPU_pop(CPU_t* cpu){
    if(!cpu) return 0xff;
    return CPU_read(cpu, 0x0100 + cpu->SP++);
}

static void CPU_setFlag(CPU_t* cpu, char flag, bool value)
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
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
            break;
    }
}

static bool CPU_getFlag(const CPU_t* cpu, char flag){
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
            fprintf(stderr, "ERROR: unknown cpu flag %c\n", flag);
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

CPU_t* CPU_create(  FNdestroy destroy,
                    FNread read,
                    FNwrite write,
                    FNdump dump){
    if(!destroy || !read || ! write || !dump) return NULL;
    CPU_t* res = (CPU_t*)malloc(sizeof(*res));
    if(!res) return NULL;
    Bus_t* bus = (Bus_t*)malloc(sizeof(*bus));
    if(!bus){
        free(res);
        return NULL;
    }
    res->bus = bus;
    res->bus->read = read;
    res->bus->write = write;
    res->bus->destroy = destroy;
    res->bus->dump = dump;
    return res;
}

void CPU_destroy(CPU_t* cpu){
    if(!cpu) return;
    cpu->bus->destroy();
    free(cpu);
}

byte CPU_read(CPU_t* cpu, word addr){
    if(!cpu) return 0xff; // open bus
    return cpu->bus->read(addr);
}

void CPU_write(CPU_t* cpu, word addr, byte value){
    if(!cpu) return;
    cpu->bus->write(addr, value);
}

bool CPU_step(CPU_t* cpu){
    if(!cpu) return false;
    // fetch:
    Opcode_t opcode = CPU_read(cpu, cpu->PC++);
    // decode:
    Instruction_t instruction = Opcode_to_Instruction_table[opcode];
    if(!instruction){
        CPU_error(cpu, "unknown opcode: 0x%.2x in PC: 0x%.4x", opcode, cpu->PC - 1);
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
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, ".\n");
    va_end(args);
}

void  CPU_dump(const CPU_t* cpu, FILE* file){
    if(!cpu || !file) return;
    cpu->bus->dump();
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

    // push return addres to stack
    CPU_push(cpu, (byte)(cpu->PC >> 8));
    CPU_push(cpu, (byte)(cpu->PC >> 0));

    // push P(flags on the stack)
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

    // push return addres to stack
    CPU_push(cpu, (byte)(cpu->PC >> 8));
    CPU_push(cpu, (byte)(cpu->PC >> 0));

    // push P(flags on the stack)
    CPU_setFlag(cpu, 'b', false);
    CPU_push(cpu, cpu->P);

    // jump to handler
    cpu->PC = addr;

    CPU_setFlag(cpu, 'i', true);
}