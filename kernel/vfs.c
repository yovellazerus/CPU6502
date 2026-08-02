
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

struct Device_Operation {
    int (*read)(File*, void*, uint16_t);
    int (*write)(File*, void*, uint16_t);
    int (*close)(File*);
};

#define MACK_DEVICE(major_number, read_fn, write_fn, close_fn)      \
    devsw_table[major_number].read = read_fn;                       \
    devsw_table[major_number].write = write_fn;                     \
    devsw_table[major_number].close = close_fn

Device_Operation devsw_table[256];

void vfs_init(void){

    memset(global_file_table, 0, sizeof(global_file_table));
    
    // manually configure the very first entry in the global file table to the console
    global_file_table[0].type = VFILE_TYPE_DEVICE;
    global_file_table[0].major = DEVICE_MAJOR_CONSOLE;
    global_file_table[0].readable = 1;
    global_file_table[0].writable = 1;
    global_file_table[0].offset = 0;
    // ...

    memset(devsw_table, 0, sizeof(devsw_table));
    
    MACK_DEVICE(DEVICE_MAJOR_CONSOLE, console_read, console_write, console_close);
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

    // 6. Dispatch the read command (fills the dynamically mapped WINDOW2)
    bytes_read = devsw_table[file->major].read(file, read_buffer, syscall_argument.read.size);
    
    // copy the data from WINDOW2 to the user's buffer
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

    
    // copy the data form the user's buffer to WINDOW2
    if(copy_from_user(write_buffer, (uint16_t)syscall_argument.write.buffer, syscall_argument.write.size, page_table) < 0){
        LOG();
        return -1;
    }

    // dispatch useing the devsw
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

