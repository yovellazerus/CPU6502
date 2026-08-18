
#include "common.h"

Super_Block sb;

File_Operations fs_ops = {
    fs_open,
    fs_close,
    fs_read,
    fs_write,
    fs_ioctl,
};

char* msg = "Hello from the kernel to disk!\n";
char* msg2 = "and i wrote this to disk to!!!\n";

void fs_init(uint8_t drive){
    frame_t old_frame;
    uint16_t block;
    Block_Buffer* buffer;

    // register the disk file system as a device driver
    devsw_table[DEVICE_MAJOR_DISK] = &fs_ops;

    // read the super block
    buffer = buffer_read(drive, SUPER_BLOCK_BLOCK);
    mmu_map_window(2, buffer->frame, &old_frame);
    memcpy(&sb, (void*)WINDOW2, sizeof(sb));
    mmu_unmap_window(2, old_frame);
    buffer_release(buffer);
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

// TODO: fs_create()

int fs_open(File* f){
    (void)f;
    panic("fs_open");
    return -1;
}

int fs_close(File* f){
    (void)f;
    panic("fs_close");
    return -1;
}

int fs_read(File* f, void* buffer, uint16_t length){
    (void)f; (void)buffer; (void)length;
    panic("fs_read");
    return -1;
}

int fs_write(File* f, void* buffer, uint16_t length){
    (void)f; (void)buffer; (void)length;
    panic("fs_write");
    return -1;
}

int fs_ioctl(File* f, uint8_t request, void* arg){
    (void)f; (void)request; (void)arg;
    panic("fs_ioctl");
    return -1;
}
