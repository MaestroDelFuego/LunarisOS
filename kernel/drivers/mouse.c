#include <stdint.h>
#include "vga.h"
#include "gui.h"

#define PS2_DATA_PORT 0x60
#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC_EOI 0x20

#define CURSOR_WIDTH 3
#define CURSOR_HEIGHT 3

typedef struct {
    char ch;
    uint8_t fg;
    uint8_t bg;
} CursorPixel;

CursorPixel cursor_shape[CURSOR_HEIGHT][CURSOR_WIDTH] = {
    { {' ', 0x0F, 0x00}, {'/', 0x0F, 0x00}, {' ', 0x0F, 0x00} },
    { {'/', 0x0F, 0x00}, {'>', 0x0F, 0x00}, {' ', 0x0F, 0x00} },
    { {' ', 0x0F, 0x00}, {'|', 0x0F, 0x00}, {' ', 0x0F, 0x00} }
};

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

volatile int mouse_x = 40, mouse_y = 12;
volatile uint8_t packet[3];
volatile int packet_idx = 0;
volatile uint8_t buttons = 0;

void mouse_handle_packet() {
    // Process buttons
    buttons = packet[0] & 0x07;
    int8_t dx = (int8_t)packet[1];
    int8_t dy = (int8_t)packet[2];

    mouse_x += dx;
    mouse_y -= dy;  // VGA y axis goes down, mouse y goes up

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= 80) mouse_x = 79;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y >= 25) mouse_y = 24;
}


void draw_cursor(int x, int y) {
    // Restore old cursor background
    mouse_restore_bg();

    // Save current chars under new cursor position
    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        int py = y + row;
        if (py < 0 || py >= VGA_HEIGHT) continue;

        for (int col = 0; col < CURSOR_WIDTH; col++) {
            int px = x + col;
            if (px < 0 || px >= VGA_WIDTH) continue;

            cursor_state.saved_chars[row][col] = vga_buffer[py * VGA_WIDTH + px];
        }
    }

    // Draw the cursor shape
    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        int py = y + row;
        if (py < 0 || py >= VGA_HEIGHT) continue;

        for (int col = 0; col < CURSOR_WIDTH; col++) {
            int px = x + col;
            if (px < 0 || px >= VGA_WIDTH) continue;

            CursorPixel pxl = cursor_shape[row][col];
            uint16_t val = (pxl.bg << 12) | (pxl.fg << 8) | pxl.ch;
            vga_buffer[py * VGA_WIDTH + px] = val;
        }
    }

    cursor_state.x = x;
    cursor_state.y = y;
    cursor_state.visible = 1;
}

__attribute__((interrupt)) void mouse_irq_handler(void* frame) {
    uint8_t data = inb(PS2_DATA_PORT);
    packet[packet_idx++] = data;

    if (packet_idx == 3) {
        packet_idx = 0;
        mouse_handle_packet();
    }

    // Send EOI to PICs
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
