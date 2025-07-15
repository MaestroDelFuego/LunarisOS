#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_clear(void);
void vga_flush(void);
void vga_print(const char* str);
void vga_draw_rect(int x, int y, int w, int h, uint8_t color);


#endif
