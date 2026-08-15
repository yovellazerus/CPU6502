
#include "common.h"

Super_Block sb;

static void read_super_block(uint8_t drive, Super_Block* sb){
    Block_Buffer* b;
    frame_t old_frame;
    
    b = buffer_read(drive, SUPER_BLOCK_BLOCK);
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
}

// TODO: the balloc(), bfree(), bzero() static functions
// TODO: fs_open(), fs_close() ... (disk file interface)
