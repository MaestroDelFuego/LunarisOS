#include "vga.h"

static size_t row = 0, col = 0;

char* vga_buffer = (char*)0xB8000;  // global definition

void vga_print(const char* str, uint8_t color) {
    while (*str) {
        *vga_buffer++ = *str++;
        *vga_buffer++ = color;
    }
}

uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

uint16_t vga_entry(unsigned char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint16_t)c;
}

void vga_clear(uint8_t color) {
    uint16_t cell = vga_entry(' ', color);
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEM[i] = cell;
    }
    row = col = 0;
}

void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        col = 0;
        if (++row == VGA_HEIGHT) row = 0;
        return;
    }
    VGA_MEM[row * VGA_WIDTH + col] = vga_entry(c, color);
    if (++col == VGA_WIDTH) {
        col = 0;
        if (++row == VGA_HEIGHT) row = 0;
    }
}

void vga_write(const char* s, uint8_t color) {
    for (size_t i = 0; s[i]; i++)
        vga_putchar(s[i], color);
}
