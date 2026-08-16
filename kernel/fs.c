
#include "common.h"

Super_Block sb;

void fs_read_super_block(uint8_t drive, Super_Block* sb){
    Block_Buffer* b;
    b = buffer_read(drive, SUPER_BLOCK_BLOCK);
    buffer_move(b, sb, (void*)WINDOW2, sizeof(*sb));
    buffer_release(b);
}

void fs_init(uint8_t drive){
    fs_read_super_block(drive, &sb);
    if(sb.magic != FS_MAGIC){
        panic("fs_init");
    }
}

// TODO: fs_create(), fs_open(), fs_close() ... (disk file interface)
