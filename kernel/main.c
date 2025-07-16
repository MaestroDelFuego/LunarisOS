#include "vga.h"
#include "gui.h"
#include "mouse.h"
#include "types.h"

void kernel_main() {
    vga_clear();
    gui_init();
    mouse_init();  // Initialize mouse hardware & IRQ
    gui_draw_taskbar();
    // Initial cursor draw
    draw_cursor(mouse_x, mouse_y);

    while (1) {
        // Ideally, this loop waits for interrupts or events.
        // For demonstration, you might refresh screen here:
        
        // Copy offscreen to VGA buffer (if using double buffering)
        gui_present();

        // Draw cursor at current position (handles restoring background)
        draw_cursor(mouse_x, mouse_y);
    }
}
