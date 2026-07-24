# 6502 virtual machine

A simple **MOS 6502 CPU and virtual machine** written in C.  
The project aims to faithfully reproduce the behavior of the 6502, including its instruction set and addressing modes, while keeping the code clear and modular.

---

## ✨ Features
- In the meantime, the CPU itself is implemented using free software from: Copyright (c) 2019 Ben Zotto
- Basic UART emulator
- Block device aka: "disk" emulator
- Operating system, using the cc65 compiler tool-chain
- Inspired by the xv6 operating system project by: Copyright (c) 2006-2024 Frans Kaashoek, Robert Morris, Russ Cox,
                        Massachusetts Institute of Technology
- ROM chip and its source code
- Bootloader
- Kernel source code directory named "kernel" (very early build...) 
- Python script to generate the disk image file form the bootloader, kernel and future applications

---

## 🔧 Build


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
├── kernel/
│   ├── boot.s
│   ├── crt0.s
│   ├── main.c
│   ├── ...
│   ├── kernel.cfg
├── mkfs.py
├── LICENSE
├── Makefile
└── README.md

