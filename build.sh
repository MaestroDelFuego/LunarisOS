#!/bin/bash
set -e

# Utility: check command exists
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

echo "Starting build process..."

# Clean old build artifacts and outputs
echo "Cleaning old build files..."
rm -rf build/* boot/kernel.bin build/myos.iso

# Check required tools
for tool in nasm gcc ld objcopy; do
  if ! command_exists "$tool"; then
    echo "Error: Required tool '$tool' not found. Please install it." >&2
    exit 1
  fi
done

if ! command_exists grub-mkrescue; then
  echo "Warning: 'grub-mkrescue' not found. ISO creation will be skipped."
  MAKE_ISO=false
else
  MAKE_ISO=true
fi

# Check source files
if [[ ! -f src/kernel/kernel.asm ]]; then
  echo "Error: src/kernel/kernel.asm not found!" >&2
  exit 1
fi

if [[ ! -f src/kernel/kernel.c ]]; then
  echo "Error: src/kernel/kernel.c not found!" >&2
  exit 1
fi

if [[ ! -f src/linker/linker.ld ]]; then
  echo "Error: src/linker/linker.ld not found!" >&2
  exit 1
fi

# Check grub.cfg
if [[ ! -f boot/grub/grub.cfg ]]; then
  echo "Error: boot/grub/grub.cfg not found!" >&2
  exit 1
fi

# Create directories
mkdir -p build boot/grub rootfs/bin rootfs/etc rootfs/lib rootfs/usr rootfs/home rootfs/tmp build/iso

echo "Assembling kernel.asm..."
if ! nasm -f elf32 src/kernel/kernel.asm -o build/kernel.o; then
  echo "Error: nasm failed" >&2
  exit 1
fi

echo "Compiling kernel.c..."
if ! gcc -m32 -ffreestanding -fno-pie -fno-pic -c src/kernel/kernel.c -o build/kernel_c.o; then
  echo "Error: gcc failed" >&2
  exit 1
fi

echo "Linking kernel..."
if ! ld -m elf_i386 -T src/linker/linker.ld build/kernel.o build/kernel_c.o -o build/kernel.elf; then
  echo "Error: ld failed" >&2
  exit 1
fi

echo "Creating flat binary..."
if ! objcopy -O binary build/kernel.elf build/kernel.bin; then
  echo "Error: objcopy failed" >&2
  exit 1
fi

echo "Copying kernel.bin to boot directory..."
cp build/kernel.bin boot/kernel.bin

echo "Preparing ISO directory structure..."
mkdir -p build/iso/boot/grub
cp boot/kernel.bin build/iso/boot/
cp boot/grub/grub.cfg build/iso/boot/grub/

if [ "$MAKE_ISO" = true ]; then
  echo "Creating bootable ISO image..."
  if ! grub-mkrescue -o build/myos.iso build/iso; then
    echo "Error: grub-mkrescue failed" >&2
    exit 1
  fi
  echo "ISO image created at build/myos.iso"
else
  echo "ISO creation skipped due to missing grub-mkrescue."
fi

echo "Build complete! To test:"
echo "  qemu-system-i386 -cdrom build/myos.iso"

qemu-system-i386 -cdrom build/myos.iso -serial file:qemu-serial.log
qemu-system-i386 -cdrom build/myos.iso -d int,cpu_reset -no-reboot -no-shutdown -serial stdio


