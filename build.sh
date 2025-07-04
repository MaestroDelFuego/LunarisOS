#!/bin/bash
set -euo pipefail

echo "[*] Cleaning old files..."
rm -rf build isodir/boot/kernel.elf lunaris.iso

echo "[*] Preparing build directories..."
mkdir -p build

echo "[*] Compiling kernel..."
gcc -m32 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c kernel.c -o build/kernel.o

echo "[*] Assembling multiboot header..."
nasm -f elf32 multiboot_header.s -o build/multiboot_header.o

echo "[*] Linking kernel..."
ld -m elf_i386 -T linker.ld -o isodir/boot/kernel.elf build/multiboot_header.o build/kernel.o

echo "[*] Creating ISO image..."
grub-mkrescue -o lunaris.iso isodir

echo "[✓] Build complete! Boot using:"
echo "    qemu-system-i386 -cdrom lunaris.iso" 

echo "[*] Starting QEMU..."
# Note: The memory size has been changed to 1096MB as per the recent edit
# This is to ensure compatibility with the latest changes in boot.sh
qemu-system-i386 -cdrom lunaris.iso -m 128 -vga std -d int,cpu_reset -no-reboot -monitor stdio
