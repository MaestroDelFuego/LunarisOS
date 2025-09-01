#ifndef PORT_H
#define PORT_H

#include <stdint.h>

// Read a byte from a port
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}
void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "dN"(port));
}


#endif
