#ifndef __ASM_H__
#define __ASM_H__

#include "defs.h"

static inline void outb(uint8_t value, uint16_t port) {
    __asm__ volatile ( \
        "outb %%al, %%dx\n" \
        : : "a"(value), "d"(port)
    );
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret = 0;
    __asm__ volatile ( \
        "inb %%dx, %%al" \
        :"=a"(ret) : "d"(port)
    );
    return ret;
}

static inline void pause() {
    asm volatile ("pause");
}

#endif // __IO_H__
