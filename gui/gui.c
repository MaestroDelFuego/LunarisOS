#include <string.h>
#include "gui.h"
#include "vga.h"

#define VGA_COLOR_BLACK 0x0
#define VGA_COLOR_BLUE  0x1
#define VGA_COLOR_LIGHT_GREY 0x7
#define VGA_COLOR_WHITE 0xF

#define VGA_ENTRY(fg, bg) (((bg) << 4) | ((fg) & 0x0F))

void gui_clear_screen(void) {
    // Clear offscreen buffer to black bg, light grey fg
    vga_draw_rect(0, 0, VGA_WIDTH, VGA_HEIGHT, VGA_ENTRY(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void gui_draw_taskbar(void) {
    // Draw taskbar background (black bg, light grey fg)
    vga_draw_rect(0, VGA_HEIGHT - 1, VGA_WIDTH, 1, VGA_ENTRY(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

    // Draw "Start" button on left side (white text, blue bg)
    const char* start_text = "Start";
    vga_draw_rect(0, VGA_HEIGHT - 1, 7, 1, VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
    vga_draw_string(1, VGA_HEIGHT - 1, start_text, VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

    // Draw time on right side (hardcoded example "12:34")
    const char* time_text = "12:34";
    int time_x = VGA_WIDTH - strlen(time_text) - 1;
    vga_draw_string(time_x, VGA_HEIGHT - 1, time_text, VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
}

void gui_present(void) {
    gui_draw_taskbar();
    vga_flush();  // Flush offscreen buffer to screen
}

void gui_init(void) {
    gui_clear_screen();
    gui_draw_taskbar();
    vga_flush();
}
