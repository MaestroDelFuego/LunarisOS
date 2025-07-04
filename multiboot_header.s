section .multiboot
    align 4
    dd 0x1BADB002
    dd 0x00010003
    dd -(0x1BADB002 + 0x00010003)

section .text
    global _start
    extern kernel_main

_start:
    cli
    xor eax, eax
    mov esp, __stack_top   ; This is now defined by linker.ld
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite
