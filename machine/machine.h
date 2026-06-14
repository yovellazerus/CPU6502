#ifndef MACHINE_H
#define MACHINE_H

// ===========================================================================================
// memory map
// ===========================================================================================

#define RAM_BASE     0x0000
#define RAM_SIZE     0xfc00

#define DISK_BUF     0xfc00 // 512 bytes
#define DISK_STAT    0xfe00
#define DISK_CMD     0xfe01
#define DISK_LBA     0xfe02

#define UART_TX      0xfe10
#define UART_RX      0xfe11
#define UART_STAT    0xfe12

#define MMU_MAP      0xfe20 // 16 bytes

#define ROM_BASE     0xff00
#define ROM_SIZE     0x0100

// ===========================================================================================
// contents
// ===========================================================================================

#define DISK_SIZE_IN_BYTES  (32*1024*1024)
#define DISK_SECTOR_SIZE    512
#define DISK_MAX_LBA        (DISK_SIZE_IN_BYTES / DISK_SECTOR_SIZE)
#define DISK_LATENCY        200 // arbitrary value...

#define CPU_PER_STEP        100  // arbitrary value...


#endif // MACHINE_H