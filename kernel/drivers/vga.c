#include "vga.h"

uint16_t* vga_buffer = (uint16_t*)0xB8000;
static uint16_t offscreen_buffer[VGA_WIDTH * VGA_HEIGHT];

static int vga_row = 0, vga_col = 0;

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        offscreen_buffer[i] = (0x07 << 8) | ' ';  // light gray on black
    }
    vga_row = 0;
    vga_col = 0;
}

void vga_flush(void) {
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

void vga_draw_string(int x, int y, const char* str, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;
    uint16_t* pos = offscreen_buffer + y * VGA_WIDTH + x;
    while (*str && (pos - offscreen_buffer) < VGA_WIDTH * VGA_HEIGHT) {
        *pos++ = (color << 8) | *str++;
    }
}
