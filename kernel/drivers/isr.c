#include "isr.h"

#define NUM_INTERRUPTS 256
isr_t interrupt_handlers[NUM_INTERRUPTS];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}
