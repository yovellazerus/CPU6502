#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

#define MEM_SIZE (64*KB)

typedef unsigned char byte;
typedef unsigned short word;

typedef struct memory{
    byte data[MEM_SIZE];  
} Memory;

void Memory_init(Memory* mem);
void Memory_dump(Memory* mem, FILE* stream);




#endif // MEMORY_H