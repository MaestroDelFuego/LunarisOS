ENTRY(start)

SECTIONS
{
  . = 0x100000;

  .multiboot ALIGN(4) : {
    KEEP(*(.multiboot))
  }

  .text : {
    *(.text)
  }

  .rodata : {
    *(.rodata)
  }

  .data : {
    *(.data)
  }

  .bss : {
    *(.bss)
    *(COMMON)
  }
}
