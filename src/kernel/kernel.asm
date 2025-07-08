; Multiboot header with correct flags and addresses

section .multiboot_header
    align 4
    dd 0x1BADB002              ; magic number
    dd 0x00010003              ; flags: ALIGN_MODULES + MEMORY_INFO + ADDRESS_FIELDS
    dd -(0x1BADB002 + 0x00010003) ; checksum

    dd multiboot_header        ; header_addr (address of this header)
    dd load_start              ; load_addr (start of kernel)
    dd load_end                ; load_end_addr (end of kernel)
    dd bss_end                 ; bss_end_addr (end of bss)
    dd entry_point             ; entry_addr (kernel entry point)

section .text
global _start
extern kernel_main

multiboot_header:
load_start:
_start:
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt

load_end:
bss_start:
    resb 4096
bss_end:
entry_point:
