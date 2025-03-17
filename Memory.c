
#include "Memory.h"

void Memory_init(Memory* mem){
    for(int i = 0; i < MEM_SIZE; i++){
        mem->data[i] = 0;
    }
}

void Memory_dump(Memory* mem, FILE* stream){
    word line = 0;
    for(int i = 0; i < MEM_SIZE; i++){
        if(i % 8 == 0){
            fprintf(stream, "\n0x%.4x: ", line);
            line += 8;
        }
        fprintf(stream, " 0x%.2x ", mem->data[i]);
    }
}