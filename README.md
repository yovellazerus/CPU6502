# 6502 virtual machine

A simple **MOS 6502 CPU and virtual machine** written in C.  
The project aims to faithfully reproduce the behavior of the 6502, including its instruction set and addressing modes, while keeping the code clear and modular.

---

## ✨ Features
- In the meantime, the CPU itself is implemented using free software from: Copyright (c) 2019 Ben Zotto
- Basic UART emulator
- Block device aka: "disk" emulator
- Operating system, using the cc65 compiler tool-chain
- ROM chip and its source code
- Bootloader
- Kernel source code directory named "kernel" (very early build...) 
- Python script to generate the disk image file form the bootloader, kernel and future applications

---

## 🔧 Build
Clone and build using `make`:

```bash
git clone https://github.com/yovellazerus/CPU6502.git
cd CPU6502
make run

## Project Structure:
.
├── src/
│   ├── cpu.c
│   ├── instruction.c
│   ├── main.c
├── include/
│   ├── ansi_codes.h
│   ├── cpu.h
│   ├── instruction.h
├── machine/
│   ├── main.c
│   ├── ...
│   ├── rom.s
│   ├── mkfs.py
├── kernel/
│   ├── boot.s
│   ├── crt0.s
│   ├── main.c
│   ├── ...
│   ├── kernel.cfg
├── LICENSE
├── Makefile
└── README.md

