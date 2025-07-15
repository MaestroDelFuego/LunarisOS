#include "vga.h"
#include "gui.h"
#include "mouse.h"

void kernel_main() {
    vga_flush();  // Ensure the VGA buffer is ready
    vga_clear();
    gui_init();
    //gui_draw_window(10, 5, 60, 15, "Neo98 GUI Window");
    //vga_flush();  // push changes to the actual VGA buffer
    gui_draw_taskbar();
    gui_present(); // <-- THIS is the key
    while (1) {
        mouse_draw(mouse_x, mouse_y);
    }
}
