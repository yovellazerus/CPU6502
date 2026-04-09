
#include "cpu.h"

typedef struct Machin_t {

    // RAM
    byte ram[32*1024];
    size_t ram_start;
    size_t ram_size;

    // ROM
    byte rom[32*1024];
    size_t rom_start;
    size_t rom_size;

} Machin_t;

Machin_t* Machin_create(void){
    Machin_t* res = calloc(1, sizeof(Machin_t));
    if(!res) return NULL;

    res->ram_start = 0x0000;
    res->ram_size  = 32*1024;
    res->rom_start = 0x8000;
    res->rom_size  = 32*1024;

    // program
    res->rom[0x7ffc] =  0x00;
    res->rom[0x7ffd] =  0x80;
    res->rom[0x0000] =  0xff;

    return res;
}

void Machin_destroy(Machin_t* m){
    free(m);
}

void Machin_dump(Machin_t* m){
    FILE* f;
    f = fopen("ram.bin", "wb");
    if(!f) return;
    fwrite(m->ram, 1, sizeof(m->ram), f);
    fclose(f);

    f = fopen("rom.bin", "wb");
    if(!f) return;
    fwrite(m->rom, 1, sizeof(m->rom), f);
    fclose(f);
}

byte Machin_read(void* ctx, word addr){
    Machin_t* m = ctx;
    if(addr >= m->ram_start && addr < m->ram_start + m->ram_size){
        return m->ram[addr - m->ram_start];
    }
    else if(addr >= m->rom_start && addr < m->rom_start + m->rom_size){
        return m->rom[addr - m->rom_start];
    }
    else{
        return 0xff; // open bus
    }
}

void Machin_write(void* ctx, word addr, byte value){
    Machin_t* m = ctx;
    if(addr >= m->ram_start && addr < m->ram_start + m->ram_size){
        m->ram[addr - m->ram_start] = value;
    }
    else if(addr >= m->rom_start && addr < m->rom_start + m->rom_size){
        return; // read only memory...
    }
    else{
        return;
    }
}

int main(void){

    Machin_t* machin = Machin_create();
    if(!machin){
        fprintf(stderr, "Machin_create");
        return 1;
    }

    Bus_t bus = {
        .ctx = machin,
        .read = Machin_read,
        .write = Machin_write,
    };
    
    CPU_t* cpu = CPU_create(bus);
    if(!cpu){
        fprintf(stderr, "CPU_create");
        return 1;
    }

    CPU_reset(cpu);
    while(CPU_step(cpu)) {}
    CPU_dump_registers(cpu, stdout);
    CPU_destroy(cpu);

    Machin_dump(machin);
    Machin_destroy(machin);

    return 0;
}
