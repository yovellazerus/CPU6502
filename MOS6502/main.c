
#include "common.h"
#include "cpu.h"

typedef struct machin_t {

    // RAM
    byte ram[32*1024];
    size_t ram_start;
    size_t ram_size;

    // ROM
    byte rom[32*1024];
    size_t rom_start;
    size_t rom_size;

} machin_t;

machin_t* machin_create(void){
    machin_t* res = calloc(1, sizeof(machin_t));
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

void destroy(void* ctx){
    free(ctx);
}

byte read(void* ctx, word addr){
    machin_t* m = ctx;
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

void write(void* ctx, word addr, byte value){
    machin_t* m = ctx;
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

void dump(void* ctx){
    machin_t* m = ctx;
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

int main(void){

    Bus_t bus = {
        .ctx = machin_create(),
        .read = read,
        .write = write,
        .dump = dump,
        .destroy = destroy,
        .reset = NULL,
    };
    
    CPU_t* cpu = CPU_create(bus);
    if(!cpu){
        fprintf(stderr, "CPU_create");
        return 1;
    }

    CPU_reset(cpu);
    while(CPU_step(cpu)) {}
    CPU_dump(cpu, stdout);
    CPU_destroy(cpu);
    return 0;
}
