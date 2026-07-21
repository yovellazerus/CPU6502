# ====================================================================================
#  Makefile for CPU6502 project (Windows 64-bit only)
# ====================================================================================

CC      = gcc
CA      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\ca65.exe
LD      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\ld65.exe
CL      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\cl65.exe
CFLAGS  = 
OBJ     = .\machine\main.o .\machine\MCS6502.o
TARGET  = .\machine\machine.exe

# Windows cleanup command
RM      = del /Q /F

.PHONY: all clean run

# ====================================================================================
#  Main build rule
# ====================================================================================
all: $(TARGET) .\machine\rom.bin .\kernel\kernel.bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)
	$(RM) $(OBJ) >nul 2>&1

.\machine\rom.bin: .\machine\rom.s .\machine\rom.cfg
	$(CA) .\machine\rom.s -o .\machine\rom.o
	$(LD) .\machine\rom.o -C .\machine\rom.cfg -o .\machine\rom.bin
	$(RM) .\machine\rom.o >nul 2>&1

.\kernel\kernel.bin:  .\kernel\crt0.s .\kernel\boot.s .\kernel\main.c .\kernel\io.c .\kernel\string.c
	$(CL) -t none -C .\kernel\kernel.cfg -o .\kernel\kernel.bin .\kernel\crt0.s .\kernel\boot.s .\kernel\main.c .\kernel\io.c .\kernel\string.c

.\machine\%.o: .\machine\%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ====================================================================================
#  Create disk image and run the virtual machine
# ====================================================================================

run:
	mingw32-make
	python.exe .\machine\mkfs.py
	.\machine\machine.exe .\machine\disk.bin

# ====================================================================================
#  Cleanup rule
# ====================================================================================
clean:
	-$(RM) $(OBJ) $(TARGET) >nul 2>&1
	-$(RM) .\machine\*.o >nul 2>&1
	-$(RM) .\machine\*.bin >nul 2>&1
	-$(RM) .\kernel\*.o >nul 2>&1
	-$(RM) .\kernel\*.bin >nul 2>&1
	-$(RM) .\bin\* >nul 2>&1

	
