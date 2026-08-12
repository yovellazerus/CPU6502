
#include "common.h"

typedef struct Cache {
    Block_Buffer head;
    Block_Buffer buffers[CACHE_SIZE];
} Cache;

Cache buffer_cache;

// look through buffer cache for block on a specific drive
// if not found, allocate a buffer
// in either case, return a locked buffer using BUFFER_FLAGS_BUSY
static Block_Buffer* buffer_get(uint8_t drive, uint16_t block_number){
    Block_Buffer* b;
    // lock the cache
    INTER_OFF();
    for(b = buffer_cache.head.next; b != &buffer_cache.head; b = b->next){

        // found it
        if(b->block_number == block_number && b->drive == drive){

            // is someone else using it or waiting for the disk?
            if(b->flags & BUFFER_FLAGS_BUSY){
                sleep((void*)b);
                // allow pending interrupts to fire, then restart search
                INTER_ON();
                return buffer_get(drive, block_number); 
            }

            // it's ours, lock it
            b->flags |= BUFFER_FLAGS_BUSY;
            b->refcount++;
            INTER_ON();
            return b;
        }
    }

    // not found, recycle an old, unused buffer from the tail
    for(b = buffer_cache.head.prev; b != &buffer_cache.head; b = b->prev){
        
        // ensure the buffer is not in use by another process
        if(b->refcount == 0 && (b->flags & BUFFER_FLAGS_BUSY) == 0){
            b->drive = drive;
            b->block_number = block_number;
            b->refcount = 1;
            b->flags = BUFFER_FLAGS_BUSY; // clear old flags, lock the new buffer
            
            INTER_ON();
            return b;
        }
    }

    panic("full cache");
    return NULL; 
}

void buffer_init(void){

    Block_Buffer* b;

    // create linked list of buffers
    buffer_cache.head.prev = &buffer_cache.head;
    buffer_cache.head.next = &buffer_cache.head;
    for(b = buffer_cache.buffers; b < buffer_cache.buffers + CACHE_SIZE; b++){
        memset(b, 0, sizeof(*b));
        b->next = buffer_cache.head.next;
        b->prev = &buffer_cache.head;
        buffer_cache.head.next->prev = b;
        buffer_cache.head.next = b;
        // pre allocating all the buffer cache dynamic memory
        b->frame = kalloc();
        if(b->frame == FRAME_UNUSED){
            panic("buffer_init");
        }
    }
}

// return a locked buffer with the contents of the indicated block
Block_Buffer* buffer_read(uint8_t drive, uint16_t block_number){
    Block_Buffer* b = buffer_get(drive, block_number);
    if((b->flags & BUFFER_FLAGS_VALID) == 0){
        disk_block_read(b);
        b->flags |= BUFFER_FLAGS_VALID;
    }
    return b;
}

// write b's contents to disk, must be locked using BUFFER_FLAGS_BUSY
void buffer_write(Block_Buffer* b){
    // check that the buffer is locked
    if((b->flags & BUFFER_FLAGS_BUSY) == 0){
        panic("buffer_write");
    }
    disk_block_write(b);
}

// release a locked buffer
// move to the head of the most-recently-used list
void buffer_release(Block_Buffer* b){
    // check that the buffer is locked
    if((b->flags & BUFFER_FLAGS_BUSY) == 0){
        panic("buffer_release");
    }

    INTER_OFF();

    // wake up any process sleeping on this buffer
    wakeup((void*)b);

    // release the sleep-lock so other processes can use it
    b->flags &= ~BUFFER_FLAGS_BUSY;

    b->refcount--;
    if(b->refcount == 0){
        // no one has a reference to it anymore
        // move to the tail of the most-recently-used list
        b->next->prev = b->prev;
        b->prev->next = b->next;
        
        b->next = buffer_cache.head.next;
        b->prev = &buffer_cache.head;
        buffer_cache.head.next->prev = b;
        buffer_cache.head.next = b;
    }

    INTER_ON();
}

void buffer_pin(Block_Buffer* b){
    INTER_OFF();
    b->refcount++;
    INTER_ON();
}

void buffer_unpin(Block_Buffer* b){
    INTER_OFF();
    b->refcount--;
    INTER_ON();
}
