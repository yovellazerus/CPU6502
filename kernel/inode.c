
#include "common.h"

// in-memory copy of an inode
struct Inode {
    uint8_t  drive;           
    uint16_t inode_number;          
    uint8_t  refcount;
    uint8_t  valid;  // inode has been read from disk?

    // copy of disk inode
    Inode_Type type; 
    uint8_t  major;
    uint8_t  minor;
    uint8_t  nlink;
    uint32_t size;
    uint8_t  data[DIRECTLY_BLOCK_COUNT + 1];
};

// On-disk inode structure
struct Dinode {
    Inode_Type type; 
    uint8_t  major;
    uint8_t  minor;
    uint8_t  nlink;
    uint32_t size;
    uint8_t  data[DIRECTLY_BLOCK_COUNT + 1];
};

struct Inode_Cache {
    Inode table[MAX_INMEMORY_INODES];
};

Inode_Cache inode_cache;

// in-memory inode table init
void inode_init(void){
    // ...
}

// TODO: inode functions...
