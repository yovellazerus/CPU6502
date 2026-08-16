
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

// helper to get the disk inode data FROM the dynamic frame
static void dinode_get(Inode* inode, Dinode* dst){
    frame_t old_frame;
    uint8_t inode_index = (uint8_t)(inode - inode_cache.inmemory);
    void* src = (void*)(WINDOW2 + inode_index * sizeof(Dinode));
    mmu_map_window(2, inode_cache.frame, &old_frame);
    memcpy(dst, src, sizeof(Dinode));
    mmu_unmap_window(2, old_frame);
}

// helper to set the disk inode data TO the dynamic frame
static void dinode_set(Inode* inode, Dinode* src){
    frame_t old_frame;
    uint8_t inode_index = (uint8_t)(inode - inode_cache.inmemory);
    void* dst = (void*)(WINDOW2 + inode_index * sizeof(Dinode));
    mmu_map_window(2, inode_cache.frame, &old_frame);
    memcpy(src, dst, sizeof(Dinode));
    mmu_unmap_window(2, old_frame);
}

void inode_init(void){
    inode_cache.frame = kalloc();
    if(inode_cache.frame == FRAME_UNUSED){
        panic("inode_init");
    }
}

// TODO: inode functions...
