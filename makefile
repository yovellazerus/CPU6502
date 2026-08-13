# ====================================================================================
#  makefile for CPU6502 and xv6502 project (Windows 64-bit)
# ====================================================================================

# --- toolchain Paths ---
CC       = gcc
CC65_BIN = C:/Users/yovel/Desktop/VScode/CPU6502/cc65-snapshot-win64/bin
CA       = $(CC65_BIN)/ca65.exe
LD       = $(CC65_BIN)/ld65.exe
CL       = $(CC65_BIN)/cl65.exe

# --- directories ---
MACH_DIR = machine
KERN_DIR = kernel
MKFS_DIR = mkfs

# --- flags ---
# Added basic warnings and optimization for the C compiler
CFLAGS   = -Wall -Wextra -O2

# --- files ---
MACH_SRC = $(MACH_DIR)/machine.c $(MACH_DIR)/MCS6502.c
MACH_OBJ = $(MACH_SRC:.c=.o)
EMULATOR = $(MACH_DIR)/machine.exe

ROM_SRC  = $(MACH_DIR)/rom.s
ROM_CFG  = $(MACH_DIR)/rom.cfg
ROM_OBJ  = $(MACH_DIR)/rom.o
ROM_BIN  = $(MACH_DIR)/rom.bin

KERN_SRC = $(wildcard $(KERN_DIR)/*.s) $(wildcard $(KERN_DIR)/*.c)
KERN_CFG = $(KERN_DIR)/kernel.cfg
KERN_BIN = $(KERN_DIR)/kernel.bin

MKFS_SRC = $(MKFS_DIR)/mkfs.c
MKFS_EXE = $(MKFS_DIR)/mkfs.exe

DISK_IMG = $(MACH_DIR)/disk.bin

# windows cleanup command (del requires backslashes)
RM       = del /Q /F

.PHONY: all clean run

# ====================================================================================
#  main build rules
# ====================================================================================
all: $(EMULATOR) $(ROM_BIN) $(KERN_BIN) $(DISK_IMG)

# emulator build
$(EMULATOR): $(MACH_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(MACH_DIR)/%.o: $(MACH_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ROM build
$(ROM_BIN): $(ROM_SRC) $(ROM_CFG)
	$(CA) $(ROM_SRC) -o $(ROM_OBJ)
	$(LD) $(ROM_OBJ) -C $(ROM_CFG) -o $@

# kernel build
$(KERN_BIN): $(KERN_SRC) $(KERN_CFG)
	$(CL) -t none -C $(KERN_CFG) -o $@ $(KERN_SRC)

# mkfs utility build (host tool)
$(MKFS_EXE): $(MKFS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# disk image build
$(DISK_IMG): $(KERN_BIN) $(MKFS_EXE)
	$(MKFS_EXE) $(KERN_BIN) $@

# ====================================================================================
#  execution
# ====================================================================================
run: all
	$(EMULATOR) $(DISK_IMG)

# ====================================================================================
#  cleanup
# ====================================================================================
clean:
	-$(RM) $(MACH_DIR)\*.o >nul 2>&1
	-$(RM) $(MACH_DIR)\*.exe >nul 2>&1
	-$(RM) $(MACH_DIR)\*.bin >nul 2>&1
	-$(RM) $(KERN_DIR)\*.o >nul 2>&1
	-$(RM) $(KERN_DIR)\*.bin >nul 2>&1
	-$(RM) $(MKFS_DIR)\*.exe >nul 2>&1