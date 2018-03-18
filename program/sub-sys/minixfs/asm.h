#ifndef __ASM_H__
#define __ASM_H__

#include "defs.h"

static inline void outb(uint8_t value, uint16_t port) {
    __asm__ volatile (
        "outb %%al, %%dx\n"
        : : "a"(value), "d"(port)
    );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret = 0;
    __asm__ volatile (
        "inb %%dx, %%al"
        :"=a"(ret) : "d"(port)
    );
    return ret;
}

static inline void outsw(uint32_t buffer, uint32_t cnt, uint16_t port) {
    __asm__ volatile (
        "cld \n"
        "rep outsw \n"
        : : "S"(buffer), "c"(cnt), "d"(port)
    );
}

static inline void insw(uint32_t cnt, uint16_t port, uint32_t buffer) {
    __asm__ volatile (
        "cld \n"
        "rep insw \n"
        : : "D"(buffer), "c"(cnt), "d"(port)
    );
}

static inline void pause() {
    asm volatile ("pause");
}

#endif // __IO_H__
