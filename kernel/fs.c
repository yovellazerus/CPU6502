
#include "common.h"

Super_Block sb;

void fs_read_super_block(uint8_t drive, Super_Block* sb){
    frame_t old_frame;
    Block_Buffer* b;
    b = buffer_read(drive, SUPER_BLOCK_BLOCK);
    mmu_map_window(2, b->frame, &old_frame);
    memcpy(sb, (void*)WINDOW2, sizeof(*sb));
    mmu_unmap_window(2, old_frame);
    buffer_release(b);
}

char* msg = "Hello from the kernel to disk!\n";
char* msg2 = "and i wrote this to disk to!!!\n";

void fs_init(uint8_t drive){
    frame_t old_frame;
    uint16_t block;
    Block_Buffer* buffer;
    fs_read_super_block(drive, &sb);
    if(sb.magic != FS_MAGIC){
        panic("fs_init");
    }

    // testing the block layer
    block = block_alloc(drive);
    buffer = buffer_read(drive, block);
    mmu_map_window(2, buffer->frame, &old_frame);
    memcpy((void*)WINDOW2, msg, strlen(msg));
    mmu_unmap_window(2, old_frame);
    buffer_write(buffer);
    buffer_release(buffer);

    block = block_alloc(drive);
    buffer = buffer_read(drive, block);
    mmu_map_window(2, buffer->frame, &old_frame);
    memcpy((void*)WINDOW2, msg2, strlen(msg2));
    mmu_unmap_window(2, old_frame);
    buffer_write(buffer);
    buffer_release(buffer);
}

// TODO: fs_create(), fs_open(), fs_close() ... (disk file interface)
