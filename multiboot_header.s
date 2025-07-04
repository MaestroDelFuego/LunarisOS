section .multiboot
    align 4
    dd 0x1BADB002                ; magic number
    dd 0x00010003                ; flags: align + memory info
    dd -(0x1BADB002 + 0x00010003) ; checksum

section .text
    global _start
    extern kernel_main
    extern __stack_top           ; symbol defined in linker script

_start:
    cli                         ; disable interrupts

    mov esp, __stack_top        ; set stack pointer from linker symbol

    call kernel_main            ; call C kernel main function

.hang:
    cli
    hlt
    jmp .hang                  ; infinite loop to halt CPU

section .note.GNU-stack noalloc noexec nowrite
