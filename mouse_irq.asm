global mouse_irq_handler_asm
extern mouse_irq_handler_c

section .text
mouse_irq_handler_asm:
    ; Save registers that IRQ handler should save
    pushad
    pushfd

    ; Call the C handler
    call mouse_irq_handler_c

    ; Restore registers
    popfd
    popad

    ; Send end of interrupt (already done in C, so optional here)
    ; iret
    ; Instead of iret, just return (this is IRQ handler; you might want iret)

    ; IRQ handlers must end with iret, so:
    iret
