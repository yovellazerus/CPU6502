
#include "common.h"

struct Super_Block {
    uint32_t magic;        // must be FS_MAGIC
    uint16_t size;         // size of file system image (in blocks)
    uint16_t block_count;  // number of data blocks
    uint16_t inode_count;  // number of inodes
    uint16_t inode_start;  // block number of first inode block
    uint16_t bitmap_start; // block number of first free bitmap block
};

struct Dir_Entry {
    char name[MAX_FILE_NAME];
    uint16_t inode_number; 
};

struct Super_Block sb;

static void read_super_block(uint8_t drive, Super_Block* sb){
    Block_Buffer* b;
    frame_t old_frame;
    
    b = buffer_read(drive, 1);
    mmu_map_window(2, b->frame, &old_frame);
    memmove(sb, (void*)WINDOW2, sizeof(*sb));
    mmu_unmap_window(2, old_frame);

    buffer_release(b);
}

void fs_init(uint8_t drive){
    read_super_block(drive, &sb);
    if(sb.magic != FS_MAGIC){
        panic("fs_init");
    }
    // TODO: xv6 uses a function called: "ireclaim"
    inode_init();
}

// TODO: the balloc(), bfree(), bzero() static functions
// TODO: fs_open(), fs_close() ... (disk file interface)
