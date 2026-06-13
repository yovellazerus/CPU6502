# ====================================================================================
#  Makefile for CPU6502 project (Windows 64-bit only)
# ====================================================================================

CC      = gcc
CA      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\ca65.exe
LD      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\ld65.exe
CL      = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-snapshot-win64\bin\cl65.exe
CFLAGS  = 
OBJ     = .\main.o .\MCS6502.o
TARGET  = .\machine.exe

# Windows cleanup command
RM      = del /Q /F

.PHONY: all clean

# ====================================================================================
#  Main build rule
# ====================================================================================
all: $(TARGET) .\rom.bin .\kernel.bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)
	$(RM) $(OBJ) >nul 2>&1

.\rom.bin: .\rom.asm .\rom.cfg
	$(CA) .\rom.asm -o .\rom.o
	$(LD) .\rom.o -C .\rom.cfg -o .\rom.bin
	$(RM) .\rom.o >nul 2>&1

# .\kernel.bin: .\boot.asm .\kernel.asm .\kernel.cfg
# 	$(CA) .\kernel.asm -o .\kernel.o
# 	$(CA) .\boot.asm -o .\boot.o
# 	$(LD) .\boot.o .\kernel.o -C .\kernel.cfg -o .\kernel.bin
# 	$(RM) .\boot.o .\kernel.o >nul 2>&1

.\kernel.bin: crt0.s boot.asm ..\kernel\main.c
	$(CL) -t none -C kernel.cfg -o kernel.bin crt0.s boot.asm ..\kernel\main.c

.\%.o: .\%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ====================================================================================
#  Cleanup rule
# ====================================================================================
clean:
	-$(RM) $(OBJ) $(TARGET) >nul 2>&1
	-$(RM) .\*.o >nul 2>&1
	-$(RM) .\*.bin >nul 2>&1
	-$(RM) .\*.bin >nul 2>&1
