
#include "common.h"

File_Operations console_ops = {
    console_open,
    console_close,
    console_read,
    console_write,
    console_ioctl,
    console_stat
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
int console_read(File* file, void* dst, uint16_t n){
    int c;
    char* dest = (char*)dst;
    int bytes_read = 0;
    (void)file;

    while(bytes_read < n){

        // modifies a the uart ring buffer that is accessible to interrupts therefore, must be locked
        INTER_OFF();
        c = uart_getc();
        if(c == -1){
            // ring buffer is empty
            sleep(&ring_buffer);
            INTER_ON();
            // very importent! if 2 processes will wakeup at the same time,
            // the first will read the character, and the second will get -1 form uart_getc()
            // and will go back to sleep
            continue;
        }
        INTER_ON();

        // BACKSPACE or DEL
        if(c == '\b' || c == 0x7f){
            if(bytes_read > 0){
                // remove from user buffer
                bytes_read--;

                // deleting the character visually
                uart_putc_sync('\b'); 
                uart_putc_sync(' ');
                uart_putc_sync('\b');
            }
            // skip adding the BACKSPACE to the ring buffer
            continue;
        }

        // TODO: need to check for '\0'?

        // is it the end of the line?
        if(c == '\r' || c == '\n'){
            // standard UNIX '\n' in the user buffer
            dest[bytes_read++] = '\n';

            uart_putc_sync('\r');
            uart_putc_sync('\n');
            break;
        }

        // normal character
        dest[bytes_read++] = c;
        // echo
        uart_putc_sync(c);
    }

    return bytes_read;
}

int console_write(File* file, void* src, uint16_t n){
    uint16_t bytes_written;
    char* source = (char*)src;
    (void)file;
    for(bytes_written = 0; bytes_written < n; bytes_written++){
        if(source[bytes_written] == '\n'){
            uart_putc('\r');
        }
        uart_putc(source[bytes_written]);
    }
    return bytes_written;
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

int console_stat(File* f, Stat* st){
    (void)f; (void)st;
    panic("console_stat");
    return -1;
}