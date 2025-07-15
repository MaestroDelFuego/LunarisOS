#ifndef GUI_H
#define GUI_H

#include <stdint.h>  // for uint16_t
#include <stdbool.h>

extern bool start_menu_open;
extern uint16_t offscreen_buffer[80 * 25];  // VGA_WIDTH * VGA_HEIGHT

// Make sure these are defined for the buffer
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Start menu dimensions
#define START_MENU_WIDTH 25
#define START_MENU_HEIGHT 6

void gui_init();
void gui_draw_window(int x, int y, int width, int height, const char* title);
void gui_draw_taskbar();
void gui_draw_start_menu(void);
void gui_toggle_start_menu(void);
void mouse_restore_bg(void);
void mouse_draw(int x, int y);  
bool gui_is_start_menu_open(void);

#endif
