#include <stdint.h>
#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA hardware buffer (actual screen memory)
uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Offscreen buffer in RAM for double buffering
static uint16_t offscreen_buffer[VGA_WIDTH * VGA_HEIGHT];

int vga_row = 0, vga_col = 0;

void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        offscreen_buffer[i] = (0x07 << 8) | ' ';  // light gray on black
    }
    vga_row = 0;
    vga_col = 0;
}

// Flush offscreen buffer to VGA hardware buffer
void vga_flush() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = offscreen_buffer[i];
    }
}

void vga_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            vga_row++;
            vga_col = 0;
        } else {
            if (vga_row < VGA_HEIGHT && vga_col < VGA_WIDTH) {
                offscreen_buffer[vga_row * VGA_WIDTH + vga_col++] = (0x07 << 8) | *str;
            }
        }
        str++;
    }
}

void vga_draw_rect(int x, int y, int w, int h, uint8_t color) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            if(row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
                offscreen_buffer[row * VGA_WIDTH + col] = (color << 8) | ' ';
            }
        }
    }
}
