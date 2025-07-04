ENTRY(_start)

SECTIONS {
  /* Start kernel at 1MB */
  . = 0x100000;

  /* Multiboot header must be in the first 8KB of the file */
  .multiboot : {
    KEEP(*(.multiboot))
  }

  .text : {
    *(.text*)
  }

  .rodata : {
    *(.rodata*)
  }

  .data : {
    *(.data*)
  }

  .bss : {
    *(.bss*)
    *(COMMON)
  }

  /* Define and reserve 16KB for the stack */
  . = ALIGN(16);
  __stack_bottom = .;
  . = . + 0x4000; /* 16KB stack */
  __stack_top = .;
}
