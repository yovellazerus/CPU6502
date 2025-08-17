# 6502 Emulator

A simple **MOS 6502 CPU emulator** written in C.  
The project aims to faithfully reproduce the behavior of the 6502, including its instruction set and addressing modes, while keeping the code clear and modular.

---

## ✨ Features
- 6502 registers: **A, X, Y, PC, SP, P (status)**
- Full set of **addressing modes**
- Instruction lookup table with function pointers
- Emulated **memory (64KB)**
- Can run raw machine code or assembled binaries
- Debug options: step execution, register/memory dump

---

## 🔧 Build
Clone and build using `make`:

```bash
git clone https://github.com/yovellazerus/CPU6502.git
cd CPU6502
make
.\cpu6502 program.bin

## Project Structure:
.
├── src/
│   ├── cpu.c
│   ├── memory.c
│   ├── instructions.c
│   ├── main.c
├── include/
│   ├── cpu.h
│   ├── memory.h
├── tests/
│   ├── test_programs/
│   └── unit_tests.c
├── Makefile
└── README.md

