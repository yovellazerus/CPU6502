
#include "comman.h"

// using cooked mode
int console_read(File* file, void* dst, uint16_t n){
    int c;
    char* dest = (char*)dst;
    int bytes_read = 0;
    (void)file;

    // leave 1 byte for the '\n'
    while(bytes_read < n - 1){
        c = uart_getc();
        
        if(c == -1){
            // ring buffer is empty
            sleep(&ring_buffer);
            // very importent! if 2 processes will wakeup at the same time,
            // the first will read the character, and the second will get -1 form uart_getc()
            // and will go back to sleep
            continue;
        }

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
        // --- NEW: Drop the lingering \n from VS Code ---
        if(c == '\n'){
            continue; 
        }

        // Handle the actual Enter key (\r)
        if(c == '\r'){
            // ALWAYS save it as a standard UNIX \n in the user buffer
            dest[bytes_read++] = '\n';

            // Echo the full CRLF back to the screen
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
    return 0;
}
