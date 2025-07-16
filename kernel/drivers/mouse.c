#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC_EOI 0x20

#define PS2_ENABLE_MOUSE 0xA8
#define PS2_WRITE_TO_MOUSE 0xD4
#define PS2_MOUSE_ENABLE_DATA_REPORTING 0xF4

#define CURSOR_WIDTH 3
#define CURSOR_HEIGHT 3

typedef struct {
    char ch;
    uint8_t fg;
    uint8_t bg;
} CursorPixel;

// Simple cursor shape: white on black
CursorPixel cursor_shape[CURSOR_HEIGHT][CURSOR_WIDTH] = {
    { {' ', 0x0F, 0x00}, {'/', 0x0F, 0x00}, {' ', 0x0F, 0x00} },
    { {'/', 0x0F, 0x00}, {'>', 0x0F, 0x00}, {' ', 0x0F, 0x00} },
    { {' ', 0x0F, 0x00}, {'|', 0x0F, 0x00}, {' ', 0x0F, 0x00} }
};

extern uint16_t* vga_buffer;  // Pointer to VGA text buffer (0xB8000)
extern void mouse_irq_handler_asm(void);  // Your asm IRQ stub calling mouse_irq_handler_c


static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Wait until the PS/2 controller input buffer is clear before sending commands
static void ps2_wait_input_clear() {
    while (inb(PS2_STATUS_PORT) & 0x02) {}  // Wait while input buffer is full
}

// Wait until output buffer is full before reading data
static void ps2_wait_output_ready() {
    while (!(inb(PS2_STATUS_PORT) & 0x01)) {}  // Wait for output buffer full
}

// Send command to PS/2 controller
static void ps2_send_command(uint8_t cmd) {
    ps2_wait_input_clear();
    outb(PS2_COMMAND_PORT, cmd);
}

// Send data to mouse device (send 0xD4 to controller, then data)
static void ps2_send_mouse_command(uint8_t cmd) {
    ps2_wait_input_clear();
    outb(PS2_COMMAND_PORT, PS2_WRITE_TO_MOUSE);
    ps2_wait_input_clear();
    outb(PS2_DATA_PORT, cmd);
}

// Enable mouse device and data reporting
void mouse_init(void) {
    // Enable auxiliary device (mouse)
    ps2_send_command(PS2_ENABLE_MOUSE);

    // Enable data reporting from mouse
    ps2_send_mouse_command(PS2_MOUSE_ENABLE_DATA_REPORTING);

    // Unmask IRQ12 on PIC2 (bit 4)
    uint8_t pic2_mask = inb(0xA1);
    pic2_mask &= ~(1 << 4);
    outb(0xA1, pic2_mask);

    // Unmask cascade IRQ2 on PIC1 (bit 2)
    uint8_t pic1_mask = inb(0x21);
    pic1_mask &= ~(1 << 2);
    outb(0x21, pic1_mask);

    // Send EOI to PICs just in case
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

// ... rest of your CursorPixel, cursor_shape, and cursor state structs ...

volatile int mouse_x = VGA_WIDTH / 2;
volatile int mouse_y = VGA_HEIGHT / 2;

volatile uint8_t packet[3];
volatile int packet_idx = 0;

typedef struct {
    int x, y;
    int visible;
    uint16_t saved_chars[CURSOR_HEIGHT][CURSOR_WIDTH];
} CursorState;

static CursorState cursor_state = {0};

void mouse_restore_bg(void) {
    if (!cursor_state.visible) return;

    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        int py = cursor_state.y + row;
        if (py < 0 || py >= VGA_HEIGHT) continue;

        for (int col = 0; col < CURSOR_WIDTH; col++) {
            int px = cursor_state.x + col;
            if (px < 0 || px >= VGA_WIDTH) continue;

            vga_buffer[py * VGA_WIDTH + px] = cursor_state.saved_chars[row][col];
        }
    }
    cursor_state.visible = 0;
}

void draw_cursor(int x, int y) {
    mouse_restore_bg();

    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        int py = y + row;
        if (py < 0 || py >= VGA_HEIGHT) continue;

        for (int col = 0; col < CURSOR_WIDTH; col++) {
            int px = x + col;
            if (px < 0 || px >= VGA_WIDTH) continue;

            cursor_state.saved_chars[row][col] = vga_buffer[py * VGA_WIDTH + px];
        }
    }

    for (int row = 0; row < CURSOR_HEIGHT; row++) {
        int py = y + row;
        if (py < 0 || py >= VGA_HEIGHT) continue;

        for (int col = 0; col < CURSOR_WIDTH; col++) {
            int px = x + col;
            if (px < 0 || px >= VGA_WIDTH) continue;

            CursorPixel pxl = cursor_shape[row][col];
            uint16_t val = ((pxl.bg & 0x0F) << 12) | ((pxl.fg & 0x0F) << 8) | pxl.ch;
            vga_buffer[py * VGA_WIDTH + px] = val;
        }
    }

    cursor_state.x = x;
    cursor_state.y = y;
    cursor_state.visible = 1;
}

void mouse_handle_packet(void) {
    int8_t dx = (int8_t)packet[1];
    int8_t dy = (int8_t)packet[2];

    mouse_x += dx;
    mouse_y -= dy;  // Usually Y is inverted

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= VGA_WIDTH) mouse_x = VGA_WIDTH - 1;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y >= VGA_HEIGHT) mouse_y = VGA_HEIGHT - 1;

    // Do NOT draw cursor here in IRQ handler! Just update position.
    // Drawing should happen in the main loop.
}

// Mouse IRQ handler called from assembly ISR
void mouse_irq_handler_c(void) {
    uint8_t data = inb(PS2_DATA_PORT);
    packet[packet_idx++] = data;

    if (packet_idx == 3) {
        packet_idx = 0;
        mouse_handle_packet();
    }

    // Send EOI to PIC
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
