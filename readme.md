NEO98
=====

NEO98 is a minimal operating system kernel designed as a learning project to explore low-level system programming. It features a basic graphical user interface (GUI) with a taskbar, VGA text mode output, and PS/2 mouse support. The project is written in C and x86 assembly, targeting a 32-bit architecture, and is built to run on real hardware or in an emulator like QEMU.

Features
--------
- VGA Text Mode GUI: Displays a taskbar with a "Start" button and real-time clock.
- PS/2 Mouse Support: Handles mouse movement and renders a simple cursor on the screen.
- Multiboot Compliance: Boots using a Multiboot-compliant bootloader (e.g., GRUB).
- Double-Buffered Graphics: Uses an offscreen buffer for smooth rendering in VGA text mode.
- Basic Standard Library: Implements minimal stdio and string functions for kernel use.

Project Structure
----------------
The project is organized as follows:

neo98/
├── gui/
│   ├── gui.c           # Core GUI functions (screen clearing, taskbar rendering)
│   └── taskbar/
│       └── taskbar.c   # Taskbar-specific rendering with real-time clock
├── kernel/
│   ├── main.c          # Kernel entry point and main loop
│   └── drivers/
│       ├── mouse.c     # PS/2 mouse driver and IRQ handler
│       └── vga.c       # VGA text mode driver with double buffering
├── lib/
│   ├── stdio.c         # Minimal stdio implementation (printf, puts, putchar)
│   └── string.c        # String utilities (strlen, memset, memcpy, strcmp)
├── include/
│   ├── gui.h           # GUI function declarations
│   ├── mouse.h         # Mouse driver declarations
│   ├── stdarg.h        # Variable argument list support
│   ├── stdbool.h       # Boolean type definitions
│   ├── stdio.h         # Standard I/O function declarations
│   ├── string.h        # String function declarations
│   ├── taskbar.h       # Taskbar function declarations
│   ├── types.h         # Type definitions (e.g., uint8_t)
│   └── vga.h           # VGA driver declarations
├── multiboot.s         # Multiboot header for bootloader compatibility
├── start.s             # Kernel entry point (calls kernel_main)
├── mouse_irq.asm       # Mouse IRQ handler in assembly
├── linker.ld           # Linker script for kernel layout
└── Makefile            # Build script for creating the bootable ISO

Prerequisites
-------------
To build and run NEO98, you need the following tools:
- NASM: Netwide Assembler for compiling assembly code (multiboot.s, start.s, mouse_irq.asm).
- GCC: GNU Compiler Collection for compiling C code with -m32 and -ffreestanding flags.
- GRUB: For creating a bootable ISO (grub-mkrescue).
- QEMU: For emulating the OS (optional, for testing).
- GNU Make: For building the project using the provided Makefile.
