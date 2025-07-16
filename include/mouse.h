#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Mouse state
extern volatile int mouse_x;
extern volatile int mouse_y;
extern volatile uint8_t mouse_buttons; // bit 0: left, bit 1: right, bit 2: middle

// Initialize mouse hardware and IRQ
void mouse_init(void);

// Mouse IRQ handler (called on mouse interrupt)
void mouse_irq_handler(void* frame);

// Draw mouse cursor at (x, y)
// Should be called regularly to update cursor rendering
void draw_cursor(int x, int y);

#endif // MOUSE_H
