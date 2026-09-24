# ====================================================================================
#  makefile for CPU6502 and xv6502 project (Cross-Platform: Windows / Linux WSL)
# ====================================================================================

# --- OS Detection & Dynamic Toolchain Paths ---
ifeq ($(OS),Windows_NT)
    # Windows environments
    CC65_BIN = C:/Users/yovel/Desktop/VScode/CPU6502/cc65-snapshot-win64/bin
    CA       = $(CC65_BIN)/ca65.exe
    LD       = $(CC65_BIN)/ld65.exe
    CL       = $(CC65_BIN)/cl65.exe
    EXE      = .exe
    RM       = del /Q /F
    FIXPATH  = $(subst /,\,$1)
    QUIET    = >nul 2>&1
else
    # Linux / WSL environments
    # In WSL, the apt packages for cc65 are automatically in the system PATH
    CA       = ca65
    LD       = ld65
    CL       = cl65
    EXE      = 
    RM       = rm -f
    FIXPATH  = $1
    QUIET    = >/dev/null 2>&1
endif

CC       = gcc
CFLAGS   = -Wall -Wextra -O2

# --- directories ---
MACH_DIR = machine
KERN_DIR = kernel
MKFS_DIR = mkfs

# --- files ---
MACH_SRC = $(MACH_DIR)/machine.c $(MACH_DIR)/MCS6502.c
MACH_OBJ = $(MACH_SRC:.c=.o)
# Dynamically append .exe on Windows, or leave blank on Linux
EMULATOR = $(MACH_DIR)/machine$(EXE) 

ROM_SRC  = $(MACH_DIR)/rom.s
ROM_CFG  = $(MACH_DIR)/rom.cfg
ROM_OBJ  = $(MACH_DIR)/rom.o
ROM_BIN  = $(MACH_DIR)/rom.bin

KERN_SRC = $(wildcard $(KERN_DIR)/*.s) $(wildcard $(KERN_DIR)/*.c)
KERN_CFG = $(KERN_DIR)/kernel.cfg
KERN_BIN = $(KERN_DIR)/kernel.bin

MKFS_SRC = $(MKFS_DIR)/mkfs.c
MKFS_EXE = $(MKFS_DIR)/mkfs$(EXE)

DISK_IMG = $(MACH_DIR)/disk.bin

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
#  cleanup (using FIXPATH macro to handle Windows backslashes dynamically)
# ====================================================================================
clean:
	-$(RM) $(call FIXPATH,$(MACH_OBJ)) $(QUIET)
	-$(RM) $(call FIXPATH,$(EMULATOR)) $(QUIET)
	-$(RM) $(call FIXPATH,$(ROM_OBJ)) $(QUIET)
	-$(RM) $(call FIXPATH,$(ROM_BIN)) $(QUIET)
	-$(RM) $(call FIXPATH,$(KERN_BIN)) $(QUIET)
	-$(RM) $(call FIXPATH,$(DISK_IMG)) $(QUIET)
	-$(RM) $(call FIXPATH,$(MKFS_EXE)) $(QUIET)
	-$(RM) $(call FIXPATH,$(KERN_DIR)/*.o) $(QUIET)