#include "stdio.h"
#include "vga.h"
#include <stdarg.h>

int putchar(int c) {
    char ch = (char)c;
    char str[2] = { ch, '\0' };
    vga_print(str);
    return c;
}

int puts(const char* str) {
    vga_print(str);
    vga_print("\n");
    return 0;
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    int written = 0;

    for (const char* p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                const char* s = va_arg(args, const char*);
                vga_print(s);
                written += strlen(s);
            } else if (*p == 'c') {
                char c = (char)va_arg(args, int);
                putchar(c);
                written++;
            } else if (*p == '%') {
                putchar('%');
                written++;
            } else {
                // unsupported format specifier, print literally
                putchar('%');
                putchar(*p);
                written += 2;
            }
        } else {
            putchar(*p);
            written++;
        }
    }

    va_end(args);
    return written;
}
