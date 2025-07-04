// kernel.c
// This file is part of LunarisOS, a simple educational operating system.
__attribute__((noreturn)) void kernel_main(void);

__attribute__((noreturn)) void kernel_main(void) {
    volatile char* video = (volatile char*)0xB8000;
    const char* msg = "Hello, LunarisOS!";
    
    // Clear first row (80 characters)
    for (int i = 0; i < 80; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x0F; // White on black
    }
    
    // Write message
    for (int i = 0; msg[i] != '\0' && i < 80; i++) {
        video[i * 2] = msg[i];
        video[i * 2 + 1] = 0x0F;
    }
    
    // Debug marker on second row
    video[80 * 2] = 'X';
    video[80 * 2 + 1] = 0x0F;
    
    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}




// This code is a simple kernel written in C for an x86 architecture.
// It defines a `kernel_main` function that serves as the entry point for the kernel.
// This function is the entry point of the kernel.
// It writes a string to the video memory at 0xB8000, which is
// the standard location for text mode video memory in x86 systems.
// The string is displayed in white on a black background.
// The kernel enters an infinite loop after writing the string to
// prevent it from exiting or returning control to any other code.
// The `__asm__ volatile ("hlt")` instruction is used to halt the CPU
// until the next interrupt, which is a common practice in kernel code
// to save power and prevent busy-waiting.
// The kernel does not perform any other operations or handle input/output.