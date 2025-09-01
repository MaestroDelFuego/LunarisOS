#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM ((uint16_t*)0xB8000)

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_LIGHT_GREY = 7,
};

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | (fg & 0x0F);
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint16_t)c;
}

static size_t row = 0, col = 0;
static uint8_t color = 0x07; // light grey on black

// Print a char at the current cursor position
void terminal_putchar(char c) {
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

// Print a string
void terminal_write(const char* s) {
    for (size_t i = 0; s[i]; i++)
        terminal_putchar(s[i]);
}

