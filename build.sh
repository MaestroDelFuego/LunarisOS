#!/bin/bash
set -euo pipefail

RED='\033[1;31m'
GREEN='\033[1;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info()    { echo -e "${YELLOW}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[✓]${NC} $1"; }
log_error()   { echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

check_command() {
    command -v "$1" &>/dev/null || log_error "'$1' is not installed. Please install it and retry."
}

log_info "Checking for required tools..."
for tool in gcc nasm ld grub-mkrescue qemu-system-i386 readelf file stat xorriso; do
    check_command "$tool"
done

log_info "Cleaning old build artifacts..."
rm -rf build isodir/boot/kernel.elf lunaris.iso

log_info "Preparing build directories..."
mkdir -p build isodir/boot/grub

log_info "Performing syntax check on kernel.c..."
gcc -m32 -ffreestanding -fsyntax-only kernel.c || log_error "Syntax errors found in kernel.c. Please fix them."

log_info "Compiling kernel.c to object file..."
gcc -m32 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -c kernel.c -o build/kernel.o

log_info "Assembling multiboot header..."
nasm -f elf32 multiboot_header.s -o build/multiboot_header.o

log_info "Linking kernel ELF executable..."
ld -m elf_i386 -T linker.ld -o isodir/boot/kernel.elf build/multiboot_header.o build/kernel.o
ld --verbose | grep -q "elf_i386" || log_error "Linker is not producing 32-bit ELF (elf_i386). Check your linker setup."

log_info "Verifying kernel.elf is a valid 32-bit ELF file..."
file isodir/boot/kernel.elf | grep -q "32-bit" || log_error "kernel.elf is not a 32-bit ELF file."

log_info "Checking for presence of Multiboot header section (.multiboot)..."
if ! readelf -S isodir/boot/kernel.elf | grep -q ".multiboot"; then
    log_error "Multiboot header section '.multiboot' NOT found in kernel.elf. Ensure multiboot_header.s is correct and linked first."
fi

MB_OFFSET_HEX=$(readelf -S isodir/boot/kernel.elf | grep .multiboot | awk '{print $5}')
MB_OFFSET=$((16#$MB_OFFSET_HEX))

log_info "Multiboot header physical file offset: 0x${MB_OFFSET_HEX} (${MB_OFFSET} decimal)"

if [ "$MB_OFFSET" -gt 8191 ]; then
    log_error "Multiboot header is located AFTER 8KB in the kernel.elf file. This is a problem because:\n"\
"This header must be within the first 8KB of the file for GRUB to find and load it correctly.\n\n"\
"Current offset: 0x${MB_OFFSET_HEX} (${MB_OFFSET} bytes)\n\n"\
"Likely causes:\n"\
"- The linker script does not place the .multiboot section at the start of the output.\n"\
"- Padding or ordering issues in the linker script.\n\n"\
"Solution:\n"\
"Adjust your linker.ld script so that the '.multiboot' section is the very first section in the linked ELF file.\n"\
"For example, put '.multiboot' at the top of the SECTIONS command."
fi

KERNEL_SIZE=$(stat -c%s isodir/boot/kernel.elf)
MAX_SIZE=$((1024 * 1024))

log_info "Checking kernel.elf file size: $KERNEL_SIZE bytes"
if [ "$KERNEL_SIZE" -gt "$MAX_SIZE" ]; then
    log_error "kernel.elf is larger than 1MB ($MAX_SIZE bytes). This might be too large for some boot loaders."
fi

log_success "kernel.elf size is acceptable."

log_info "Creating GRUB configuration file (grub.cfg)..."
cat > isodir/boot/grub/grub.cfg <<EOF
set debug="all"
menuentry "LunarisOS" {
    multiboot /boot/kernel.elf
    boot
}
EOF

log_info "Building bootable ISO image (lunaris.iso)..."
grub-mkrescue -o lunaris.iso isodir > /dev/null 2>&1 || log_error "Failed to create ISO image."

log_info "Verifying kernel.elf exists inside the ISO..."
if ! xorriso -indev lunaris.iso -ls /boot/kernel.elf >/dev/null 2>&1; then
    log_error "kernel.elf not found inside ISO at /boot/kernel.elf. Check ISO creation."
fi

log_info "Mounting ISO temporarily to manually verify contents..."
mkdir -p /tmp/lunaris_iso
sudo mount -o loop lunaris.iso /tmp/lunaris_iso || log_error "Failed to mount ISO image."
if [ ! -f /tmp/lunaris_iso/boot/kernel.elf ]; then
    sudo umount /tmp/lunaris_iso
    log_error "kernel.elf missing inside mounted ISO filesystem."
fi
sudo umount /tmp/lunaris_iso
log_success "kernel.elf successfully verified inside ISO."

log_info "Displaying ELF section headers for kernel.elf:"
objdump -h isodir/boot/kernel.elf

log_success "Build completed successfully! You can boot your OS using:"
echo "    qemu-system-i386 -cdrom lunaris.iso"

log_info "Launching QEMU emulator..."
qemu-system-i386 -cdrom lunaris.iso -m 128 -vga std -d int,cpu_reset -no-reboot -monitor stdio -serial file:output.log
