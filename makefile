# ====================================================================================
#  Makefile for CPU6502 project (Windows 64-bit only)
# ====================================================================================

CC      = gcc
CA      = .\cc65-2.19\bin\ca65.exe
LD      = .\cc65-2.19\bin\ld65.exe
CFLAGS  =
OBJ     = src\main.o src\cpu.o src\instruction.o
TARGET  = cpu6502.exe

# Windows cleanup command
RM      = del /Q /F

.PHONY: all clean

# ====================================================================================
#  Main build rule
# ====================================================================================
all: $(TARGET) input\bios.bin input\boot.bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)
	$(RM) $(OBJ) >nul 2>&1

input\bios.bin: input\bios.asm input\bios.cfg
	$(CA) input\bios.asm -o input\bios.o
	$(LD) input\bios.o -C input\bios.cfg -o input\bios.bin
	$(RM) input\bios.o >nul 2>&1

input\boot.bin: input\boot.asm input\boot.cfg
	$(CA) input\boot.asm -o input\boot.o
	$(LD) input\boot.o -C input\boot.cfg -o input\boot.bin
	$(RM) input\boot.o >nul 2>&1

src\%.o: src\%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ====================================================================================
#  Cleanup rule
# ====================================================================================
clean:
	-$(RM) $(OBJ) $(TARGET) >nul 2>&1
	-$(RM) input\*.o >nul 2>&1
	-$(RM) input\*.bin >nul 2>&1
