
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../kernel/common.h"

#define DISK_SIZE DISK_SIZE_IN_BYTES

#define BOOT_SRC_OFFSET   0x00000200
#define BOOT_DST_OFFSET   0x00000000
#define BOOT_SIZE         0x00000200

#define KERNEL_SRC_OFFSET 0x00003000
#define KERNEL_DST_OFFSET 0x00003000
#define KERNEL_SIZE       (0x0000FC00 - 0x00003000)

#define SB_DST_OFFSET     SUPER_BLOCK_BLOCK

// bit manipulation helpers
#define SET_BIT(bitmap, b)   ((bitmap)[(b)  >> 3] |=  (1 << ((b) % 8)))
#define CLEAR_BIT(bitmap, b) ((bitmap)[(b)  >> 3] &= ~(1 << ((b) % 8)))
#define TEST_BIT(bitmap, b)  (((bitmap)[(b) >> 3] &   (1 << ((b) % 8))) != 0)

void write_bitmap(FILE* dst, const Super_Block* sb, uint16_t root_data_block, uint16_t bitmap_blocks) {
    uint32_t bitmap_size_bytes = (uint32_t)bitmap_blocks * BLOCK_SIZE;
    uint8_t* bitmap = (uint8_t*)calloc(1, bitmap_size_bytes);
    if (!bitmap) {
        fprintf(stderr, "Error: Failed to allocate memory for bitmap\n");
        exit(1);
    }

    // mark blocks from 0 up to and including root_data_block as used (bit = 1)
    // this protects: boot, kernel, superblock, inode table, bitmap, and the root directory data block
    for (uint32_t b = 0; b <= root_data_block; b++) {
        SET_BIT(bitmap, b);
    }

    // if total disk blocks < total bits tracked by the bitmap blocks,
    // mark all out-of-range blocks as used so block_alloc() will never allocate them
    uint32_t total_bits = bitmap_size_bytes * 8;
    for (uint32_t b = sb->size; b < total_bits; b++) {
        SET_BIT(bitmap, b);
    }

    // seek to the bitmap region on disk and flush
    uint32_t bitmap_offset = (uint32_t)sb->bitmap_start * BLOCK_SIZE;
    fseek(dst, bitmap_offset, SEEK_SET);
    fwrite(bitmap, 1, bitmap_size_bytes, dst);

    free(bitmap);
}

Super_Block sb;
Dinode root_inode;

int main(int argc, char *argv[]) {

    // =============== open the disk output image ===============================================

    // default paths assume we are running from inside the "mkfs" directory
    const char *img_path = "../kernel/kernel.bin";
    const char *disk_path = "../machine/disk.bin";

    // allow overriding via command line arguments
    if (argc >= 3) {
        img_path = argv[1];
        disk_path = argv[2];
    }

    FILE *dst = fopen(disk_path, "wb");
    if (!dst) {
        perror("Error opening disk output file");
        return 1;
    }

    // truncate the disk to 32MB
    fseek(dst, DISK_SIZE - 1, SEEK_SET);
    fputc('\0', dst);

    // =================== kernel and bootloader =============================================

    FILE *src = fopen(img_path, "rb");
    if (!src) {
        fclose(dst);
        perror("Error opening kernel image");
        return 1;
    }

    // allocate buffers
    uint8_t *boot = malloc(BOOT_SIZE);
    uint8_t *kernel = malloc(KERNEL_SIZE);
    if (!boot || !kernel) {
        fclose(dst);
        fclose(src);
        printf("Memory allocation failed\n");
        return 1;
    }

    fseek(src, BOOT_SRC_OFFSET, SEEK_SET);
    fread(boot, 1, BOOT_SIZE, src);
    fseek(dst, BOOT_DST_OFFSET, SEEK_SET);
    fwrite(boot, 1, BOOT_SIZE, dst);
    free(boot);

    fseek(src, KERNEL_SRC_OFFSET, SEEK_SET);
    fread(kernel, 1, KERNEL_SIZE, src);
    fseek(dst, KERNEL_DST_OFFSET, SEEK_SET);
    fwrite(kernel, 1, KERNEL_SIZE, dst);
    free(kernel);

    // =================== superblock ===============================================================

    uint16_t total_blocks   = DISK_SIZE_IN_BYTES / BLOCK_SIZE; 
    uint16_t inode_number   = MAX_INODE_IN_DISK;               
    uint16_t inodes_per_blk = BLOCK_SIZE / sizeof(Dinode);
    uint16_t inode_blocks   = inode_number / inodes_per_blk;
    
    uint16_t bitmap_blocks  = (total_blocks + (BLOCK_SIZE * 8) - 1) / (BLOCK_SIZE * 8);

    Super_Block sb;
    memset(&sb, 0, sizeof(Super_Block));
    sb.magic        = FS_MAGIC;
    sb.size         = total_blocks;
    sb.inode_count  = inode_number;
    sb.inode_start  = SUPER_BLOCK_BLOCK + 1;
    sb.bitmap_start = sb.inode_start + inode_blocks;
    sb.block_count  = total_blocks - (sb.bitmap_start + bitmap_blocks);

    // write superblock
    fseek(dst, SUPER_BLOCK_BLOCK * BLOCK_SIZE, SEEK_SET);
    fwrite(&sb, 1, sizeof(Super_Block), dst);

    // =================== root directory ============================================================

    uint16_t data_start_block  = sb.bitmap_start + bitmap_blocks; // Block 34
    uint16_t root_data_block   = data_start_block;

    uint32_t root_inode_offset = (sb.inode_start * BLOCK_SIZE) + (ROOT_INODE * sizeof(Dinode));
    uint32_t root_data_offset  = ((uint32_t)root_data_block * BLOCK_SIZE);

    Dinode root_inode;
    memset(&root_inode, 0, sizeof(Dinode));
    root_inode.type    = INODE_TYPE_DIR; // directory
    root_inode.mode    = 0x01FF;
    root_inode.uid     = 0;
    root_inode.gid     = 0;
    root_inode.nlink   = 1;
    root_inode.size    = 2 * sizeof(Dir_Entry); // TODO: only "." and ".." sub directories for now
    root_inode.data[0] = root_data_block;

    Dir_Entry root_entries[2];
    memset(root_entries, 0, sizeof(root_entries));
    root_entries[0].inode_number = ROOT_INODE;
    strcpy(root_entries[0].name, ".");
    root_entries[1].inode_number = ROOT_INODE;
    strcpy(root_entries[1].name, "..");

    // write inode table entry
    fseek(dst, root_inode_offset, SEEK_SET);
    fwrite(&root_inode, 1, sizeof(Dinode), dst);

    // write root directory data block
    fseek(dst, root_data_offset, SEEK_SET);
    fwrite(root_entries, 1, sizeof(root_entries), dst);

    // =================== bitmap generation ==========================================================

    write_bitmap(dst, &sb, root_data_block, bitmap_blocks);

    // ========================== epilogue ============================================================

    printf("Disk image \"%s\" successfully generated (%d MB)\n", disk_path, DISK_SIZE / (1024 * 1024));

    fclose(src);
    fclose(dst);

    return 0;
}