// minimal_std.h
// Minimal definitions for bare-metal kernels

// Fixed-width integer types (like stdint.h)
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;
typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

// Size type and pointer difference (like stddef.h)
typedef unsigned int        size_t;
typedef int                 ptrdiff_t;
typedef int                 intptr_t;
typedef unsigned int        uintptr_t;

// NULL pointer
#define NULL ((void*)0)
