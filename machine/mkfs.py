
DISK_SIZE = 32 * 1024 * 1024

BOOT_SRC_OFFSET = 0xF000
BOOT_DST_OFFSET = 0x0000
BOOT_SIZE = 0x0200

KERNEL_SRC_OFFSET = 0x0200
KERNEL_DST_OFFSET = 0x0200
KERNEL_SIZE = 0xF000 - 0x0200


def mkfs(img_path, disk_path):
    with open(img_path, "rb") as src:
        boot = None
        kernel = None

        src.seek(BOOT_SRC_OFFSET)
        boot = src.read(BOOT_SIZE)

        src.seek(KERNEL_SRC_OFFSET)
        kernel = src.read(KERNEL_SIZE)

    with open(disk_path, "wb") as dst:
        dst.truncate(DISK_SIZE)

        dst.seek(BOOT_DST_OFFSET)
        dst.write(boot)

        dst.seek(KERNEL_DST_OFFSET)
        dst.write(kernel)
    

if __name__ == "__main__":
    mkfs(".\\kernel\\kernel.bin", ".\\machine\\disk.bin")