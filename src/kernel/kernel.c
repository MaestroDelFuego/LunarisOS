// Simple kernel main - prints "Hello, World!" to VGA text buffer

void kernel_main() {
    volatile char *video = (char *)0xb8000;
    const char *msg = "Hello, World!";

    for (int i = 0; msg[i] != '\0'; i++) {
        video[i*2] = msg[i];
        video[i*2 + 1] = 0x07; // light grey on black background
    }

    while (1) {} // halt here
}
