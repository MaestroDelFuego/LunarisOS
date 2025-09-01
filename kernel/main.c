#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "vga.h"
#include "keyboard.h"

void kernel_main(void) {
    uint8_t color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear(color);

    vga_write("Simple text kernel\n", color);
    vga_write("Type commands below:\n> ", color);

    init_keyboard();  // <<< must be called before reading input

    char buffer[80];
    size_t len = 0;

    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            buffer[len] = 0;
            vga_putchar('\n', color);

            if (strcmp(buffer, "clear") == 0) {
                vga_clear(color);
            } else {
                vga_write("You typed: ", color);
                vga_write(buffer, color);
                vga_putchar('\n', color);
            }

            len = 0;
            vga_write("> ", color);
        } else if (c == '\b') {
            if (len > 0) {
                len--;
                if (len < 80) vga_putchar('\b', color);
            }
        } else {
            buffer[len++] = c;
            vga_putchar(c, color);
        }
    }
}
