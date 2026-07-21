
#include "comman.h"

// static void disk_read_sector(uint8_t* buffer, uint16_t lba){
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_READ;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy(buffer, (uint8_t*)DISK_BUF, DISK_SECTOR_SIZE);
// }

// static void disk_write_sector(uint8_t* buffer, uint16_t lba){
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     MMIO16(DISK_LBA) = lba;
//     MMIO8(DISK_CMD) = DISK_CMD_WRITE;
//     while(!(MMIO8(DISK_STAT) & DISK_STATUS_READY)) {/* busy wait */};
//     memcpy((uint8_t*)DISK_BUF, buffer , DISK_SECTOR_SIZE);
// }

// ======= main =====================================================================================

void main(void) {

    int x = -42;

    printf("****** 6502 kernel ******\n");
    
    panic("number: %d, hex: 0x%x, unsigned: %u, char: %c, string: \"%s\", pointer: %p %%\n",
                    x,      0xffcb,        0xffff,     'G',    "hello world!",      &x  
    );

    while (true) {/* halt */}
}
