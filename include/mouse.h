// mouse.h
#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Current mouse position
extern volatile int mouse_x;
extern volatile int mouse_y;

// Current mouse button states (bit 0=left, bit1=right, bit2=middle)
extern volatile uint8_t mouse_buttons;

// Initialize the mouse hardware and IRQ handler
void mouse_init(void);

// The IRQ handler called on mouse interrupts
void mouse_irq_handler(void* frame);

// Draw the mouse cursor at the given position
// This function should be called in the main loop to update the cursor position
void draw_cursor(int x, int y);

#endif // MOUSE_H
