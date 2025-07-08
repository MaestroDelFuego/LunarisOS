OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
  . = 0x00100000; /* Load address 1MB */

  .multiboot_header ALIGN(4) : {
    *(.multiboot_header)
  }

  .text ALIGN(4) : {
    *(.text)
  }

  .rodata ALIGN(4) : {
    *(.rodata)
  }

  .data ALIGN(4) : {
    *(.data)
  }

  .bss ALIGN(4) : {
    *(.bss)
    *(COMMON)
  }
}
