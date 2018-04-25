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
    __asm__ volatile ("pause");
}

static inline void cli() {
    __asm__ volatile ("cli");
}

static inline void sti() {
    __asm__ volatile ("sti");
}

static inline void lidt(void* idtr) {
    __asm__ volatile (
        "lidt (%%eax) \n"
        : :"a"(idtr)
    );
}

static inline void lgdt(void* gdtr) {
    __asm__ volatile (
        "lgdt (%%eax) \n"
        : :"a"(gdtr)
    );
}

static inline void ltr(uint32_t ltr) {
    __asm__ volatile (
        "ltr %%ax \n"
        : :"a"(ltr)
    );
}

static inline void lldt(uint32_t ldt) {
    __asm__ volatile (
        "lldt %%ax \n"
        : :"a"(ldt)
    );
}

static inline void reload_sregs(uint16_t cs, uint16_t ds) {
    __asm__ volatile (
        "pushl %%eax \n"
        "pushl $1f \n"
        "retf \n"
        "1: movw %%bx, %%ds \n"
        "movw %%bx, %%es \n"
        "movw %%bx, %%fs \n"
        "movw %%bx, %%gs \n"
        "movw %%bx, %%ss \n"
        : :"a"(cs), "b"(ds)
    );
}

static inline void memcpy(void *dest, void *src, uint32_t size) {
    __asm__ volatile (
        "cld \n"
        "rep movsb \n"
        : : "S"(src), "D"(dest), "c"(size)
    );
}

static inline void switch_to_user(
    uint32_t code_sel, uint32_t data_sel, void* user_stack, void* entry) {
    __asm__ volatile (
        "pushl %0 \n"
        "pushl %1 \n"
        "pushf \n"
        "pushl %2 \n"
        "pushl %3 \n"
        "iret"
        : :"m"(data_sel), "m"(user_stack), "m"(code_sel), "m"(entry)
    );
}

#endif // __IO_H__
