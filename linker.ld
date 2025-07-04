ENTRY(_start)

SECTIONS
{
  /* Place the multiboot header at the start */
  .multiboot ALIGN(4) : {
    KEEP(*(.multiboot))
  }

  /* Start the kernel load address at 1MB */
  . = 0x100000;
  
  /* Place code here */
  .text ALIGN(4) : {
    *(.text*)
  }

  .rodata ALIGN(4) : {
    *(.rodata*)
  }

  .data ALIGN(4) : {
    *(.data*)
  }

  .bss ALIGN(4) : {
    *(.bss*)
    *(COMMON)
  }

  /* Define the stack top and bottom symbols */
  . = ALIGN(16);
  __stack_top = .;
  . = . + 0x4000;  /* reserve 16KB for stack */
  __stack_bottom = .;
}
