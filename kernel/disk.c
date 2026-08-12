
#include "common.h"

typedef struct {
    Block_Buffer* head;    // the current buffer the disk is working on
    Block_Buffer* tail;
    uint8_t sector_offset; // tracks partial block reads/writes (0 to BLOCK_SIZE / DISK_SECTOR_SIZE - 1)
} Disk;

Disk disk;

void disk_init(void){
    disk.head = disk.tail = NULL;
}

static void disk_start_operation(void) {
    uint16_t lba;
    frame_t old_frame;
    Block_Buffer* b = disk.head;

    if(b == NULL) return;

    lba = (b->block_number * (BLOCK_SIZE / DISK_SECTOR_SIZE)) + disk.sector_offset;
    
    // if this is a write command, we must copy memory to the DISK_BUF before starting the hardware
    if(b->flags & BUFFER_FLAGS_DIRTY){
        mmu_map_window(2, b->frame, &old_frame);
        memcpy((void*)DISK_BUF, (void*)(WINDOW2 + disk.sector_offset * DISK_SECTOR_SIZE), DISK_SECTOR_SIZE);
        mmu_unmap_window(2, old_frame);
        
        MMIO16(DISK_LBA) = lba;
        MMIO8(DISK_CMD) = DISK_CMD_WRITE;
    } 
    // if it is a read command, just start the hardware
    else{
        MMIO16(DISK_LBA) = lba;
        MMIO8(DISK_CMD) = DISK_CMD_READ;
    }
}

// function to enqueue the request (FIFO)
static void disk_submit(Block_Buffer* b) {
    INTER_OFF();
    
    b->queue = NULL; // ensure it doesn't point to garbage

    // append to queue
    if(disk.head == NULL){
        disk.head = b;
    }
    else{
        disk.tail->queue = b;
    }
    disk.tail = b;

    // first in line
    if(disk.head == b){
        disk.sector_offset = 0;
        disk_start_operation();
    }

    // sleep until the interrupt handler wakeup()
    sleep((void*)b);
    
    INTER_ON();
}

void disk_block_read(Block_Buffer* b) {
    // it's a read
    b->flags &= ~BUFFER_FLAGS_DIRTY; 
    disk_submit(b);
}

void disk_block_write(Block_Buffer* b) {
    // it's a write
    b->flags |= BUFFER_FLAGS_DIRTY; 
    disk_submit(b);
}

void disk_interrupt(void) {
    Block_Buffer* b;
    frame_t old_frame;

    // acknowledge the interrupt
    MMIO8(PLIC_INTERRUPT_LINES) &= ~PLIC_PIN_DISK;

    // for hardware validation
    if(MMIO8(DISK_STAT) & DISK_STATUS_READY){

        b = disk.head;
        if(b == NULL){
            return;
        }

        // if we just finished a read, we must copy data from the MMIO buffer to the frame
        if ((b->flags & BUFFER_FLAGS_DIRTY) == 0) {
            mmu_map_window(2, b->frame, &old_frame);
            memcpy((void*)(WINDOW2 + disk.sector_offset * DISK_SECTOR_SIZE), (void*)DISK_BUF, DISK_SECTOR_SIZE);
            mmu_unmap_window(2, old_frame);
        }

        // next sector
        disk.sector_offset++;

        // done with this block?

        // yes
        if(disk.sector_offset >= (BLOCK_SIZE / DISK_SECTOR_SIZE)){
            
            b->flags |= BUFFER_FLAGS_VALID;  // the buffer now contained valid data 
            b->flags &= ~BUFFER_FLAGS_DIRTY; // it is updated with the disk

            // pop it from the queue
            disk.head = b->queue;

            // wake up the process waiting for this buffer
            wakeup((void*)b);

            // if someone else is in line, start their transfer
            if(disk.head != NULL){
                disk.sector_offset = 0;
                disk_start_operation();
            }
        } 
        // no
        else{
            disk_start_operation();
        }
    }
}