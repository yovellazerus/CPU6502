
CC = gcc

CFLAGS = -Wall -Wextra -std=c99 -g

SRC = testMain.c Memory.c CPU.c operation.c
OBJ = $(SRC:.c=.o)

TARGET = mos6502

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q testMain.o Memory.o CPU.o operation.o mos6502.exe 2>nul
