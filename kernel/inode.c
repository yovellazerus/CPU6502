
#include "common.h"

// in-memory copy of an inode
struct Inode {
    uint8_t  drive;           
    uint16_t inode_number;          
    uint8_t  refcount;
    uint8_t  flags;  // inode has been read from disk? is it dirty? is it busy?
};

struct Inode_Cache {
    Inode inmemory[MAX_INMEMORY_INODES];
    frame_t frame; // for the Dinode array
};

Inode_Cache inode_cache;

void inode_init(void){
    inode_cache.frame = kalloc();
    if(inode_cache.frame == FRAME_UNUSED){
        panic("inode_init");
    }
}

// TODO: inode functions...
