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
	.\cc65-2.19\bin\ca65.exe .\input\kernel.asm -o .\input\kernel.o
	.\cc65-2.19\bin\ld65.exe .\input\kernel.o -C .\input\kernel.cfg -o .\input\kernel.bin

%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)
