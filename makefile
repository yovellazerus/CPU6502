CC = gcc
CFLAGS = # -Wall -Wextra -O2
OBJ = main.o instruction.o cpu.o
TARGET = cpu6502
RM = rm -f

ifeq ($(OS),Windows_NT)
    RM = del /Q
    TARGET = cpu6502.exe
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	$(RM) $(OBJ)
	.\cc65-2.19\bin\ca65.exe .\input\bios.asm -o .\input\bios.o
	.\cc65-2.19\bin\ld65.exe .\input\bios.o -C .\input\bios.cfg -o .\input\bios.bin

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)
