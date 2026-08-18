
#include "common.h"

File_Operations console_ops = {
    console_open,
    console_close,
    console_read,
    console_write,
    console_ioctl,
};

// TODO: using console_open?
void console_init(void) {

    // register the device
    devsw_table[DEVICE_MAJOR_CONSOLE] = &console_ops;
    
    // manually open the first entry in the global file table to the console
    global_file_table[0].offset = 0;
    global_file_table[0].inode  = NULL;       // TODO: console inode implementation
    global_file_table[0].readable = true;
    global_file_table[0].writable = true;
    global_file_table[0].fops = devsw_table[DEVICE_MAJOR_CONSOLE];
}

// using cooked mode
int console_read(File* file, void* user_dst, uint16_t n){
    int c;
    int bytes_read = 0;
    uint8_t buffer[128];
    
    // prevent buffer overflows if user asks for less than the buffer
    int max_read = (n < sizeof(buffer)) ? n : sizeof(buffer);
    (void)file;

    while(bytes_read < max_read){
        INTER_OFF();
        c = uart_getc();
        if(c == -1){
            sleep(&ring_buffer);
            INTER_ON();
            continue;
        }
        INTER_ON();

        if(c == '\b' || c == 0x7f){
            if(bytes_read > 0){
                bytes_read--;
                uart_putc_sync('\b'); 
                uart_putc_sync(' ');
                uart_putc_sync('\b');
            }
            continue;
        }

        // end of the line
        if(c == '\r' || c == '\n'){
            buffer[bytes_read++] = '\n';
            uart_putc_sync('\r');
            uart_putc_sync('\n');
            break;
        }

        // normal character
        buffer[bytes_read++] = c;
        uart_putc_sync(c);
    }

    // the line is ready (or max bytes hit), copy to user space
    if(copy_to_user(buffer, (uint16_t)user_dst, bytes_read, current_process) < 0){
        return -1;
    }

    return bytes_read;
}

int console_write(File* file, void* user_src, uint16_t n){
    uint16_t chunk_size;
    uint16_t i;
    uint8_t buffer[128];
    uint16_t total_written = 0;
    uint16_t user_ptr = (uint16_t)user_src;
    (void)file;

    while(total_written < n){

        // calculate the chunk size for this iteration
        chunk_size = ((n - total_written) < sizeof(buffer)) ? (n - total_written) : sizeof(buffer);
        
        // copy one chunk FROM user space into the driver's buffer
        if(copy_from_user(buffer, user_ptr, chunk_size, current_process) < 0){
            if(total_written == 0) return -1;
            break; // something was written before faulting
        }

        // move the chunk to the hardware driver
        for(i = 0; i < chunk_size; i++){
            if(buffer[i] == '\n'){
                uart_putc('\r'); // for standard UNIX terminals
            }
            uart_putc(buffer[i]);
        }
        
        total_written += chunk_size;
        user_ptr      += chunk_size;
    }
    
    return total_written;
}

int console_close(File* file){
    (void)file;
    panic("console_close");
    return -1;
}

int console_ioctl(File* f, uint8_t request, void* arg){
    (void)f; (void)request; (void)arg;
    panic("console_ioctl");
    return -1;
}

int console_open(File* f){
    (void)f;
    panic("console_open");
    return -1;
}
