
#include "common.h"
#include "cpu.h"

byte ram[32*1024];
byte rom[32*1024];

void destroy(void){
    // no need
}

byte read(word addr){
    if(0 <= addr && addr < 32*1024){
        return ram[addr];
    }
    else if(32*1024 <= addr && addr < 64*1024){
        return rom[addr - 32*1024];
    }
    else{
        return 0xff; // open bus
    }
}

void write(word addr, byte value){
    if(0 <= addr && addr < 32*1024){
        ram[addr] = value;
    }
    else if(32*1024 <= addr && addr < 64*1024){
        return; // read only memory...
    }
    else{
        return;
    }
}

void dump(void){
    FILE* f;
    f = fopen("ram.bin", "wb");
    if(!f) return;
    fwrite(ram, 1, sizeof(ram), f);
    fclose(f);

    f = fopen("rom.bin", "wb");
    if(!f) return;
    fwrite(rom, 1, sizeof(rom), f);
    fclose(f);
}

int main(void){
    
    CPU_t* cpu = CPU_create(destroy, read, write, dump);
    if(!cpu){
        fprintf(stderr, "CPU_create");
        return 1;
    }

    rom[0x7ffc] =  0x00;
    rom[0x7ffd] =  0x80;
    rom[0x0000] =  0xff;

    CPU_reset(cpu);
    while(CPU_step(cpu)) {}
    CPU_dump(cpu, stdout);
    CPU_destroy(cpu);
    return 0;
}
