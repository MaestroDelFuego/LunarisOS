#include "keyboard.h"
#include "isr.h"
#include "port.h"

#define KEYBOARD_DATA_PORT 0x60

// Tiny US keymap
static char keymap[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
};

static unsigned char key_state[128] = {0};
static char key_buffer = 0;

// Keyboard IRQ handler
void keyboard_handler(registers_t *regs) {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80) {
        // Key release
        scancode &= 0x7F;
        key_state[scancode] = 0;
    } else {
        // Key press
        if (!key_state[scancode]) {
            key_state[scancode] = 1;
            key_buffer = keymap[scancode]; // store key until read
        }
    }

    // End of Interrupt (EOI)
    outb(0x20, 0x20);
}

// Returns last key pressed (non-blocking)
char keyboard_getchar(void) {
    char c = key_buffer;
    key_buffer = 0; // consume key
    return c;
}

void init_pic() {
    // Initialize master and slave PIC
    outb(0x20, 0x11); // Start initialization
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // Map IRQ0-7 to interrupt 32-39
    outb(0xA1, 0x28); // Map IRQ8-15 to interrupt 40-47
    outb(0x21, 0x04); // Set up cascading
    outb(0xA1, 0x02);
    outb(0x21, 0x01); // 8086 mode
    outb(0xA1, 0x01);
    outb(0x21, 0xFD); // Unmask IRQ1 (keyboard)
    outb(0xA1, 0xFF); // Mask all others for simplicity
}

// Initialize keyboard
void init_keyboard() {
    init_pic();
    register_interrupt_handler(33, keyboard_handler); // IRQ1
    __asm__ volatile("sti"); // Enable interrupts
}
