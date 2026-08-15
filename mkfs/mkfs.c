
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../kernel/common.h"

#define DISK_SIZE (32 * 1024 * 1024) // 32 MB

#define BOOT_SRC_OFFSET   0x0200
#define BOOT_DST_OFFSET   0x0000
#define BOOT_SIZE         0x0200

#define SB_DST_OFFSET     SUPER_BLOCK_BLOCK

#define KERNEL_SRC_OFFSET 0x3000
#define KERNEL_DST_OFFSET 0x3000
#define KERNEL_SIZE       (0xFC00 - 0x3000)

Super_Block sb;

int main(int argc, char *argv[]) {
    // default paths assume we are running from inside the "mkfs" directory
    const char *img_path = "../kernel/kernel.bin";
    const char *disk_path = "../machine/disk.bin";

    // allow overriding via command line arguments (useful for Makefiles)
    if (argc >= 3) {
        img_path = argv[1];
        disk_path = argv[2];
    }

    FILE *src = fopen(img_path, "rb");
    if (!src) {
        perror("Error opening kernel image");
        return 1;
    }

    FILE *dst = fopen(disk_path, "wb");
    if (!dst) {
        perror("Error opening disk output file");
        fclose(src);
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

    // read bootloader
    fseek(src, BOOT_SRC_OFFSET, SEEK_SET);
    fread(boot, 1, BOOT_SIZE, src);

    // read kernel
    fseek(src, KERNEL_SRC_OFFSET, SEEK_SET);
    fread(kernel, 1, KERNEL_SIZE, src);

    // truncate/apd the disk to 32MB
    fseek(dst, DISK_SIZE - 1, SEEK_SET);
    fputc('\0', dst);

    // write bootloader
    fseek(dst, BOOT_DST_OFFSET, SEEK_SET);
    fwrite(boot, 1, BOOT_SIZE, dst);

    // write superblock
    uint16_t total_blocks = DISK_SIZE_IN_BYTES / BLOCK_SIZE; 
    uint16_t inodes_per_block = BLOCK_SIZE / sizeof(Dinode);
    uint16_t inode_blocks = MAX_INODE_IN_DISK / inodes_per_block;
    uint16_t bitmap_blocks = (total_blocks / (BLOCK_SIZE * 8)) + 1;

    sb.magic = FS_MAGIC;
    sb.size = total_blocks;
    sb.inode_count = MAX_INODE_IN_DISK;
    // ich region start immediately after the previs one
    sb.inode_start = SUPER_BLOCK_BLOCK + 1;
    sb.bitmap_start = sb.inode_start + inode_blocks;
    sb.block_count = total_blocks - (sb.bitmap_start + bitmap_blocks);

    fseek(dst, SB_DST_OFFSET * BLOCK_SIZE, SEEK_SET);
    fwrite(&sb, 1, sizeof(Super_Block), dst);

    // write kernel
    fseek(dst, KERNEL_DST_OFFSET, SEEK_SET);
    fwrite(kernel, 1, KERNEL_SIZE, dst);

    printf("Disk image \"%s\" successfully generated (%d MB)\n", disk_path, DISK_SIZE / (1024 * 1024));

    // cleanup
    free(boot);
    free(kernel);
    fclose(src);
    fclose(dst);

    return 0;
}