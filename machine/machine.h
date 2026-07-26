#ifndef MACHINE_H
#define MACHINE_H

// ===========================================================================================
// memory map
// ===========================================================================================

#define RAM_BASE     0x00000000
#define RAM_SIZE     0x00100000 // 1MB

#define DISK_BUF     0x0000fc00 // 512 bytes
#define DISK_STAT    0x0000fe00
#define DISK_CMD     0x0000fe01
#define DISK_LBA     0x0000fe02

#define UART_TX      0x0000fe10
#define UART_RX      0x0000fe11
#define UART_STAT    0x0000fe12

#define MMU_PAGE_TABLE  0x0000fe20 // 16 bytes

#define TIMER_ENABLE        0x0000fe40
#define TIMER_DISABLE       0x0000fe41            
#define TIMER_LATCH_LOW     0x0000fe42
#define TIMER_LATCH_HIGH    0x0000fe43
#define TIMER_COUNTER_LOW   0x0000fe44
#define TIMER_COUNTER_HIGH  0x0000fe45
#define TIMER_CTRL          0x0000fe46


#define ROM_ENABLE  0x0000fef0
#define ROM_BASE     0x0000ff00
#define ROM_SIZE     0x0100

// ===========================================================================================
// contents
// ===========================================================================================

#define DISK_SIZE_IN_BYTES  (32*1024*1024)
#define DISK_SECTOR_SIZE    512
#define DISK_MAX_LBA        (DISK_SIZE_IN_BYTES / DISK_SECTOR_SIZE)
#define DISK_LATENCY        200 // arbitrary value...

#define CPU_PER_STEP        100  // arbitrary value...

#define MMU_PAGE_TABLE_SIZE 16
#define MMU_LAST_SEGMENT    (MMU_PAGE_TABLE_SIZE - 1)
#define MMU_FRAME_UNUSED    0x00

typedef enum {
    UART_STATUS_RX_READY = 0x01,
    UART_STATUS_TX_READY = 0x02,
} Uart_Status;

typedef enum {
    DISK_CMD_NONE   = 0,
    DISK_CMD_READ   = 1,
    DISK_CMD_WRITE  = 2,
} Disk_Cmd;

typedef enum {
    ROM_ENABLE_FALSE = 0,
    ROM_ENABLE_TRUE = 1,
} ROM_Status;

typedef enum {
    TIME_ENABLE_FALSE = 0,
    TIME_ENABLE_TRUE = 1,
} Timer_Ctrl;

typedef enum {
    DISK_STATUS_NONE  = 0,
    DISK_STATUS_BUSY  = 1,
    DISK_STATUS_READY = 2,
    DISK_STATUS_ERROR = 3,
} Disk_Status;


#endif // MACHINE_H