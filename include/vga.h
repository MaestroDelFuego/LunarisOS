#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((uint16_t*)0xB8000)

enum vga_color { VGA_COLOR_BLACK = 0, VGA_COLOR_LIGHT_GREY = 7 };


extern char* vga_buffer;   // declaration for other files
void vga_print(const char* str, uint8_t color);

uint8_t vga_entry_color(uint8_t fg, uint8_t bg);
uint16_t vga_entry(unsigned char c, uint8_t color);

void vga_clear(uint8_t color);
void vga_putchar(char c, uint8_t color);
void vga_write(const char* s, uint8_t color);

#endif
