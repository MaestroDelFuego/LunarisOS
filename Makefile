ISO_NAME = neo98.iso
BUILD_DIR = build/iso
KERNEL = $(BUILD_DIR)/boot/kernel.bin

C_SOURCES = kernel/main.c kernel/drivers/vga.c kernel/drivers/mouse.c gui/gui.c lib/string.c lib/stdio.c kernel/drivers/keyboard.c kernel/drivers/isr.c
OBJECTS = kernel/main.o kernel/drivers/vga.o kernel/drivers/mouse.o gui/gui.o lib/string.o lib/stdio.o kernel/drivers/keyboard.o kernel/drivers/isr.o start.o multiboot.o
OTHER_OBJS = kernel/main.o kernel/drivers/vga.o gui/gui.o lib/string.o lib/stdio.o kernel/drivers/mouse.o kernel/drivers/keyboard.o kernel/drivers/isr.o


ASM_BOOT_OBJS = multiboot.o start.o

all: clean iso

%.o: %.c
	i686-elf-gcc -ffreestanding -m32 -nostdlib -Iinclude -c $< -o $@

%.o: %.s
	i686-elf-as $< -o $@

kernel: $(OBJECTS)
	mkdir -p $(BUILD_DIR)/boot
	i686-elf-ld -T linker.ld -o $(KERNEL) $(ASM_BOOT_OBJS) $(OTHER_OBJS)

iso: kernel
	mkdir -p $(BUILD_DIR)/boot/grub
	cp boot/grub.cfg $(BUILD_DIR)/boot/grub/
	grub-mkrescue -o $(ISO_NAME) $(BUILD_DIR)

run: iso
	qemu-system-i386 -cdrom $(ISO_NAME)

clean:
	rm -rf build *.iso *.o
