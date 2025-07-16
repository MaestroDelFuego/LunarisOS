#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern uint16_t* vga_buffer;

// Initialize VGA (clear buffers, reset cursor)
void vga_clear(void);

// Flush offscreen buffer to VGA memory (hardware)
void vga_flush(void);

// Print string at current cursor position, handling newlines
void vga_print(const char* str);

// Draw rectangle filled with spaces in given color (color is 8-bit attribute)
void vga_draw_rect(int x, int y, int w, int h, uint8_t color);

// Draw string at specified position with given 8-bit color
void vga_draw_string(int x, int y, const char* str, uint8_t color);

#endif
