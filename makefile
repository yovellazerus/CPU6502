# ====================================================================================
#  Makefile for CPU6502 project (Windows 64-bit)
# ====================================================================================

# --- Toolchain Paths ---
CC       = gcc
CC65_BIN = C:/Users/yovel/Desktop/VScode/CPU6502/cc65-snapshot-win64/bin
CA       = $(CC65_BIN)/ca65.exe
LD       = $(CC65_BIN)/ld65.exe
CL       = $(CC65_BIN)/cl65.exe
PYTHON   = python

# --- Directories ---
MACH_DIR = machine
KERN_DIR = kernel

# --- Flags ---
# Added basic warnings and optimization for the C compiler
CFLAGS   = -Wall -Wextra -O2

# --- Files ---
MACH_SRC = $(MACH_DIR)/main.c $(MACH_DIR)/MCS6502.c
MACH_OBJ = $(MACH_SRC:.c=.o)
EMULATOR = $(MACH_DIR)/machine.exe

ROM_SRC  = $(MACH_DIR)/rom.s
ROM_CFG  = $(MACH_DIR)/rom.cfg
ROM_OBJ  = $(MACH_DIR)/rom.o
ROM_BIN  = $(MACH_DIR)/rom.bin

KERN_SRC = $(KERN_DIR)/crt0.s $(KERN_DIR)/boot.s $(KERN_DIR)/trampoline.s $(KERN_DIR)/main.c $(KERN_DIR)/io.c $(KERN_DIR)/string.c $(KERN_DIR)/proc.c $(KERN_DIR)/trap.c $(KERN_DIR)/kalloc.c
KERN_CFG = $(KERN_DIR)/kernel.cfg
KERN_BIN = $(KERN_DIR)/kernel.bin

DISK_IMG = $(MACH_DIR)/disk.bin

# Windows cleanup command (del requires backslashes)
RM       = del /Q /F

.PHONY: all clean run

# ====================================================================================
#  Main build rules
# ====================================================================================
all: $(EMULATOR) $(ROM_BIN) $(KERN_BIN) $(DISK_IMG)

# 1. Emulator build
$(EMULATOR): $(MACH_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(MACH_DIR)/%.o: $(MACH_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 2. ROM build
$(ROM_BIN): $(ROM_SRC) $(ROM_CFG)
	$(CA) $(ROM_SRC) -o $(ROM_OBJ)
	$(LD) $(ROM_OBJ) -C $(ROM_CFG) -o $@

# 3. Kernel build
$(KERN_BIN): $(KERN_SRC) $(KERN_CFG)
	$(CL) -t none -C $(KERN_CFG) -o $@ $(KERN_SRC)

# 4. Disk Image build
$(DISK_IMG): $(KERN_BIN) $(MACH_DIR)/mkfs.py
	$(PYTHON) $(MACH_DIR)/mkfs.py

# ====================================================================================
#  Execution
# ====================================================================================
run: all
	$(EMULATOR) $(DISK_IMG)

# ====================================================================================
#  Cleanup
# ====================================================================================
clean:
	-$(RM) $(MACH_DIR)\*.o >nul 2>&1
	-$(RM) $(MACH_DIR)\*.exe >nul 2>&1
	-$(RM) $(MACH_DIR)\*.bin >nul 2>&1
	-$(RM) $(KERN_DIR)\*.o >nul 2>&1
	-$(RM) $(KERN_DIR)\*.bin >nul 2>&1