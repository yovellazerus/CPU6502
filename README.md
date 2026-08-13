# xv6502: A UNIX-like Operating System for a 6502 custom built computer emulator

**⚠️ PROJECT STATUS: VERY EARLY BUILD ⚠️**  
*This project is currently in its early stages of active development. Much of the architecture described below outlines the target roadmap and ongoing implementation.*

## Overview
This project aims to build a fully functional, UNIX-like, preemptive multitasking operating system from scratch, targeting a custom 8-bit MOS 6502 hardware emulator. 

The goal is to bring modern operating system concepts such as Memory Management Units (MMU), Virtual File Systems (VFS), preemptive scheduling, and POSIX-style process life cycles (`fork`, `wait`, `exit`, `kill`)—to a vintage 8-bit microprocessor architecture.

## Vision and Roadmap
Currently, the kernel demonstrates robust hardware interrupt handling, context switching, UART ring-buffering, and basic VFS chunking. The overarching vision is to transition from a monolithic test build into a true, standalone OS ecosystem.

**The Target Boot Lifecycle:**
1. **ROM (`rom.bin`):** Initializes the hardware and loads Sector 0 of the disk into `$00000200`.
2. **Stage 1 Bootloader:** A 512-byte assembly payload that sets up the C runtime and loads the second stage from disk.
3. **Stage 2 Bootloader:** A C-based loader equipped with a read-only filesystem driver. It locates `/kernel.img` on the disk, loads it into kernel memory space, and jumps to it.
4. **Kernel Initialization:** The kernel sets up the MMU, traps, and process tables, ultimately spawning the `/bin/init` user land process.
5. **User land:** `init` executes `/bin/sh`, providing an interactive shell capable of executing external binaries like `ls` and `cat`.

## Acknowledgments & Credits

*   **[xv6](https://pdos.csail.mit.edu/6.828/2012/xv6.html):** Heavily inspired by MIT's educational operating system. Many of the architectural choices regarding the UNIX process lifecycle (`proc` structures, zombie reaping, trap framing) and the VFS layout trace their lineage to xv6 design philosophies. Copyright (c) 2006-2024 Frans Kaashoek, Robert Morris, Russ Cox,
                        Massachusetts Institute of Technology
*   **The cc65 Toolchain:** This project relies entirely on the [cc65](https://cc65.github.io/) cross-compiler suite to bring C programming to the 6502. Specifically, it uses `ca65` for assembling hardware-level routines, `ld65` for precise memory segment linking (crucial for OS development), and the `cl65` frontend to tie the C and Assembly build process together. Building a UNIX-like OS on an 8-bit chip would be nearly impossible without the incredible, ongoing work of the cc65 open-source community.
*   **MCS6502:** The core CPU instruction execution within the `machine/` emulator is powered by the excellent open-source `MCS6502` emulator library. Copyright (c) 2019 by Ben Zotto

## Project Structure
The repository is strictly modularized to separate hardware emulation, boot sequence, kernel space, and user land applications.

```text
.
├── LICENSE
├── Makefile
├── README.md
│
├── mkfs/              # file system generator
│   └── mkfs.c
│
├── machine/           # The 6502 Hardware Emulator
│   ├── MCS6502.[c|h]  # 3rd-party 6502 CPU emulation core
│   ├── machine.[c|h]  # Custom hardware MMIO, MMU, UART, PLIC
│   ├── ...
│   ├── rom.s          # Hardware ROM initialization
│   └── rom.cfg        # ROM linker script
│
├── boot/             # Two-Stage Bootloader
│   ├── first_stage.s # Stage 1: 512-byte sector 0 loader
│   ├── boot.c        # Stage 2: C-based kernel locator/loader
│   ├── ...
│   └── boot.cfg            
│
├── kernel/         # The Operating System Kernel
│   ├── common.h
│   ├── trampoline.s
│   ├── main.c              
│   ├── proc.c              
│   ├── trap.c              
│   ├── vfs.c               
│   ├── mmu.c               
│   ├── ...
│   └── kernel.cfg          
│
└── user/            # User land Programs
    ├── init.c              
    ├── sh.c                
    ├── ls.c                
    ├── ...
    └── user.cfg            