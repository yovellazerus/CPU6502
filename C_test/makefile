# Bare-metal cc65 Makefile using cl65 (Win64)

# Tools
CL65 = ..\cc65-snapshot-win64\bin\cl65.exe

# Files
CFILES = main.c
SFILES = crt0.s
BIN = main.bin
LDFILE = linker.cfg

# Default target
all: $(BIN)

# Build everything with cl65
$(BIN): $(CFILES) $(SFILES) $(LDFILE)
	$(CL65) -t none -C $(LDFILE) -o $@ $(SFILES) $(CFILES)

# Clean
clean:
	del /Q $(BIN) *.o 2>nul

.PHONY: all clean
