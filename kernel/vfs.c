
#include "comman.h"

/*
core UNIX file abstraction implementation
*/

struct File {
    VFile_Type type;
    Device_Major major;
    uint8_t  readable;
    uint8_t  writable;
    uint32_t offset;
    // ...
};

File global_file_table[MAX_GLOBAL_OPEN_FILES];

File* file_get_by_global_index(uint8_t index){
    if(index < ARRAY_SIZE(global_file_table)){
        return &global_file_table[index];
    }
    return NULL;
}

bool file_open_global(uint8_t index,
                        VFile_Type type,
                        Device_Major major,
                        uint8_t  readable,
                        uint8_t  writable,
                        uint32_t offset)
{
    File* file = file_get_by_global_index(index);
    if(!file) return false;
    file->type     = type;
    file->major    = major;
    file->readable = readable;
    file->writable = writable;
    file->offset   = offset;
    return true;
}                

Device_Ops devsw_table[MAX_REGISTER_DEVICES];

bool register_device(Device_Major major, Device_Ops* devops){
    if(major >= ARRAY_SIZE(devsw_table)){
        return false;
    }
    memcpy(&devsw_table[major], devops, sizeof(Device_Ops));
    return true;
}

void vfs_init(void){

    memset(global_file_table, 0, sizeof(global_file_table));
    memset(devsw_table, 0, sizeof(devsw_table));
    
    console_init();
    // ...
}

/*
Unified system call interface for devices and regular files:
*/

int sys_read(void){
    SyscallArg syscall_argument;
    File* file;
    int bytes_read;
    uint8_t buffer_frame;
    uint8_t old_frame;
    char* read_buffer = (char*)WINDOW2;
    uint16_t ax = proc_get_ax(current_process);
    const uint8_t* page_table = proc_get_page_table(current_process);

    // populate the system call argument struct
    if(ax >= proc_get_top(current_process) || copy_from_user(&syscall_argument, ax, sizeof(syscall_argument), page_table) < 0){
        LOG();
        return -1;
    }

    // validate fd and permissions
    if(syscall_argument.read.fd < 0 || syscall_argument.read.fd >= MAX_FILES_PER_PROC){
        LOG();
        return -1;
    }
    
    file = proc_get_file(current_process, syscall_argument.read.fd);
    if(!file || !file->readable){
        LOG();
        return -1;
    }

    // limit size to the maximum size of a single frame
    if(syscall_argument.read.size > 4096){
        syscall_argument.read.size = 4096; 
    }

    // allocate a physical frame for the buffer
    buffer_frame = kalloc();
    if(buffer_frame == FRAME_UNUSED){
        LOG();
        return -1;
    }

    // map the new frame to WINDOW2
    old_frame = MMIO8(MMU_PAGE_TABLE + 2);
    MMIO8(MMU_PAGE_TABLE + 2) = buffer_frame;

    // dispatch useing the devsw (fill the dynamic buffer with the read data)
    bytes_read = devsw_table[file->major].read(file, read_buffer, syscall_argument.read.size);
    
    // copy the data from the dynamic buffer to the user's buffer
    if(copy_to_user(read_buffer, (uint16_t)syscall_argument.read.buffer, bytes_read, (uint8_t*)page_table) < 0){
        LOG();
        bytes_read = -1;
    } else {
        file->offset += bytes_read;
    }

    // remap and free the physical frame
    MMIO8(MMU_PAGE_TABLE + 2) = old_frame; 
    kfree(buffer_frame);

    return bytes_read;
}

int sys_write(void){
    SyscallArg syscall_argument;
    File* file;
    int bytes_written;
    uint8_t buffer_frame;
    uint8_t old_frame;
    char* write_buffer = (char*)WINDOW2;
    uint16_t ax = proc_get_ax(current_process);
    const uint8_t* page_table = proc_get_page_table(current_process);

    // populate the system call argument struct
    if(ax >= proc_get_top(current_process) || copy_from_user(&syscall_argument, ax, sizeof(syscall_argument), page_table) < 0){
        LOG();
        return -1;
    }

    // validate fd and permissions
    if(syscall_argument.write.fd < 0 || syscall_argument.write.fd >= MAX_FILES_PER_PROC){
        LOG();
        return -1;
    }
    
    file = proc_get_file(current_process, syscall_argument.write.fd);
    if(!file || !file->writable){
        LOG();
        return -1;
    }

    // limit size to the maximum size of a single frame
    if(syscall_argument.write.size > 4096){
        syscall_argument.write.size = 4096; 
    }

    // allocate a physical frame for the buffer
    buffer_frame = kalloc();
    if(buffer_frame == FRAME_UNUSED){
        LOG();
        return -1;
    }

    // map the new frame to WINDOW2
    old_frame = MMIO8(MMU_PAGE_TABLE + 2);
    MMIO8(MMU_PAGE_TABLE + 2) = buffer_frame;

    
    // copy the data form the user's buffer to the dynamic buffer
    if(copy_from_user(write_buffer, (uint16_t)syscall_argument.write.buffer, syscall_argument.write.size, page_table) < 0){
        LOG();
        return -1;
    }

    // dispatch useing the devsw (write the data form the dynamic buffer)
    bytes_written = devsw_table[file->major].write(file, write_buffer, syscall_argument.write.size);
    
    file->offset += bytes_written;

    // remap and free the physical frame
    MMIO8(MMU_PAGE_TABLE + 2) = old_frame; 
    kfree(buffer_frame);

    return bytes_written;
}

int sys_close(void){
    panic("sys_close");
    return 0;
}

int sys_open(void){
    panic("sys_open");
    return 0;
}

