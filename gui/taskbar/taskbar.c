#include <string.h>
#include "taskbar.h"   // Header you'll create next
#include "vga.h"       // Your VGA related defines & vga_buffer declaration
#include "types.h"

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} BiosTime;

// BIOS interrupt call to get current time
BiosTime bios_get_time() {
    BiosTime t = {0, 0, 0};
    uint16_t ax, cx, dx;

    __asm__ volatile (
        "mov $0x02, %%ah \n"
        "int $0x1a      \n"
        "jnc 1f         \n"
        "xor %%ax, %%ax \n"
        "xor %%cx, %%cx \n"
        "xor %%dx, %%dx \n"
        "1:             \n"
        : "=a"(ax), "=c"(cx), "=d"(dx)
        :
        : "cc"
    );

    t.hour = (cx >> 8) & 0xFF;    // CH = hour
    t.minute = cx & 0xFF;         // CL = minute
    t.second = (dx >> 8) & 0xFF;  // DH = second

    return t;
}

void gui_draw_taskbar() {
    int taskbar_height = 1;
    int y = VGA_HEIGHT - taskbar_height;

    // Draw taskbar background (blue)
    vga_draw_rect(0, y, VGA_WIDTH, taskbar_height, VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

    // Draw "Start" button on left side with white text on blue background
    const char* start_text = " Start ";
    int start_len = strlen(start_text);
    for (int i = 0; i < start_len; i++) {
        vga_buffer[y * VGA_WIDTH + i] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE) << 8) | start_text[i];
    }

    BiosTime t = bios_get_time();

    // Format time string as HH:MM:SS
    char time_str[9];
    time_str[0] = '0' + (t.hour / 10);
    time_str[1] = '0' + (t.hour % 10);
    time_str[2] = ':';
    time_str[3] = '0' + (t.minute / 10);
    time_str[4] = '0' + (t.minute % 10);
    time_str[5] = ':';
    time_str[6] = '0' + (t.second / 10);
    time_str[7] = '0' + (t.second % 10);
    time_str[8] = '\0';

    // Draw time string on the right side of the taskbar
    int time_len = strlen(time_str);
    int time_start = VGA_WIDTH - time_len;
    for (int i = 0; i < time_len; i++) {
        vga_buffer[y * VGA_WIDTH + time_start + i] = (VGA_ENTRY(VGA_COLOR_WHITE, VGA_COLOR_BLUE) << 8) | time_str[i];
    }
}
