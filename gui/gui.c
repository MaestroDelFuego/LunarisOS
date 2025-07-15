#include "gui.h"
#include "vga.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


// Start menu dimensions
#define START_MENU_WIDTH 20
#define START_MENU_HEIGHT 8

// VGA color definitions
#define VGA_COLOR_BLACK         0x0
#define VGA_COLOR_BLUE          0x1
#define VGA_COLOR_GREEN         0x2
#define VGA_COLOR_CYAN          0x3
#define VGA_COLOR_RED           0x4
#define VGA_COLOR_MAGENTA       0x5
#define VGA_COLOR_BROWN         0x6
#define VGA_COLOR_LIGHT_GREY    0x7
#define VGA_COLOR_DARK_GREY     0x8
#define VGA_COLOR_LIGHT_BLUE    0x9
#define VGA_COLOR_LIGHT_GREEN   0xA
#define VGA_COLOR_LIGHT_CYAN    0xB
#define VGA_COLOR_LIGHT_RED     0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_YELLOW        0xE
#define VGA_COLOR_WHITE         0xF

// Helper macro to build VGA attribute byte
#define VGA_ENTRY(fg, bg) (((bg) << 4) | ((fg) & 0x0F))

extern uint16_t* vga_buffer;

// Global offscreen buffer and state
uint16_t offscreen_buffer[VGA_WIDTH * VGA_HEIGHT];
bool start_menu_open = false;

// Mouse position (in character cells)
volatile int mouse_x = VGA_WIDTH / 2;
volatile int mouse_y = VGA_HEIGHT / 2;

// To save what was under the mouse cursor so we can restore it
static int last_mouse_x = -1;
static int last_mouse_y = -1;
static uint16_t saved_mouse_bg = 0;

// Draw filled rectangle on offscreen buffer
static void vga_draw_rect_offscreen(int x, int y, int w, int h, uint8_t attribute) {
    for (int row = y; row < y + h; row++) {
        if (row < 0 || row >= VGA_HEIGHT) continue;
        for (int col = x; col < x + w; col++) {
            if (col < 0 || col >= VGA_WIDTH) continue;
            offscreen_buffer[row * VGA_WIDTH + col] = (attribute << 8) | ' ';
        }
    }
}

// Restore old mouse background at last mouse position
static void mouse_restore_bg() {
    if (last_mouse_x >= 0 && last_mouse_x < VGA_WIDTH &&
        last_mouse_y >= 0 && last_mouse_y < VGA_HEIGHT) {
        offscreen_buffer[last_mouse_y * VGA_WIDTH + last_mouse_x] = saved_mouse_bg;
    }
}

// Save background under the mouse cursor
static void mouse_save_bg(int x, int y) {
    saved_mouse_bg = offscreen_buffer[y * VGA_WIDTH + x];
}

// Draw mouse cursor at (x, y) as a white 'X' on light red background
static void mouse_draw(int x, int y) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) return;

    mouse_restore_bg();

    last_mouse_x = x;
    last_mouse_y = y;

    mouse_save_bg(x, y);

    offscreen_buffer[y * VGA_WIDTH + x] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_RED) << 8) | 'X';
}

// Draw title bar with blue background and centered white text
static void gui_draw_title_bar(int x, int y, int w, const char* title) {
    vga_draw_rect_offscreen(x, y, w, 1, VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

    int title_len = strlen(title);
    int start_col = x + (w - title_len) / 2;

    for (int i = 0; i < title_len; i++) {
        int pos = y * VGA_WIDTH + start_col + i;
        if (pos >= 0 && pos < VGA_WIDTH * VGA_HEIGHT)
            offscreen_buffer[pos] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE) << 8) | title[i];
    }
}

// Initialize the GUI (fill screen blue)
void gui_init() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        offscreen_buffer[i] = (VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_BLUE) << 8) | ' ';
    }
    gui_present(); // Show initial screen
}

// Draw a window with light gray background, dark gray borders, and a blue title bar
void gui_draw_window(int x, int y, int w, int h, const char* title) {
    if (w <= 0 || h <= 0) return;

    // Draw window background (light gray)
    vga_draw_rect_offscreen(x, y + 1, w, h - 1, VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY));

    // Draw title bar
    gui_draw_title_bar(x, y, w, title);

    // Draw left and right borders (dark gray)
    for (int row = y + 1; row < y + h; row++) {
        if (row < 0 || row >= VGA_HEIGHT) continue;
        if (x >= 0 && x < VGA_WIDTH)
            offscreen_buffer[row * VGA_WIDTH + x] = (VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_DARK_GREY) << 8) | ' ';
        int right_x = x + w - 1;
        if (right_x >= 0 && right_x < VGA_WIDTH)
            offscreen_buffer[row * VGA_WIDTH + right_x] = (VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_DARK_GREY) << 8) | ' ';
    }
}

// Draw taskbar at bottom of screen
void gui_draw_taskbar() {
    int y = VGA_HEIGHT - 1;
    const char* os_name = "LunarisOS";

    // Taskbar background (blue)
    vga_draw_rect_offscreen(0, y, VGA_WIDTH, 1, VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_BLUE));

    // Draw start button "[Start]"
    const char* start_text = "[Start]";
    int start_len = strlen(start_text);
    int start_x = 1;

    // Draw button background (bright blue)
    for (int i = 0; i < start_len + 2; i++) {
        offscreen_buffer[y * VGA_WIDTH + start_x - 1 + i] = (VGA_ENTRY(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_BLUE) << 8) | ' ';
    }
    // Draw button text (white on bright blue)
    for (int i = 0; i < start_len; i++) {
        offscreen_buffer[y * VGA_WIDTH + start_x + i] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_BLUE) << 8) | start_text[i];
    }

    // Draw OS name centered (white on blue)
    int len = strlen(os_name);
    int pos = (VGA_WIDTH - len) / 2;
    for (int i = 0; i < len; i++) {
        offscreen_buffer[y * VGA_WIDTH + pos + i] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE) << 8) | os_name[i];
    }

    // Draw time and date on the right side (white on blue)
    const char* datetime = "12:00 07/13/2025";
    int dt_len = strlen(datetime);
    int dt_pos = VGA_WIDTH - dt_len - 1;
    for (int i = 0; i < dt_len; i++) {
        offscreen_buffer[y * VGA_WIDTH + dt_pos + i] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE) << 8) | datetime[i];
    }
}

// Draw start menu window and its items
void gui_draw_start_menu() {
    if (!start_menu_open) return;

    int x = 0;
    int y = VGA_HEIGHT - START_MENU_HEIGHT - 1;
    const char* title = "Start Menu";

    gui_draw_window(x, y, START_MENU_WIDTH, START_MENU_HEIGHT, title);

    const char* items[] = {
        "1. Programs",
        "2. Settings",
        "3. Shutdown",
    };

    int item_count = sizeof(items) / sizeof(items[0]);

    for (int i = 0; i < item_count && i < START_MENU_HEIGHT - 2; i++) {
        int pos_y = y + 1 + i;
        int pos_x = x + 2;
        const char* item = items[i];
        int len = strlen(item);

        for (int j = 0; j < len && (pos_x + j) < (x + START_MENU_WIDTH - 1); j++) {
            offscreen_buffer[pos_y * VGA_WIDTH + pos_x + j] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_LIGHT_GREY) << 8) | item[j];
        }
    }
}

void gui_toggle_start_menu() {
    start_menu_open = !start_menu_open;
}

void gui_present() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = offscreen_buffer[i];
    }
}

bool gui_is_start_menu_open() {
    return start_menu_open;
}
