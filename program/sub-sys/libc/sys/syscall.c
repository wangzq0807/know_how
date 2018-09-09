#include "syscall.h"

#define SYSCALL(num, cnt)   \
({                          \
    register int ret;       \
    __asm__ (               \
        "pushl %2 \n"       \
        "int $0x80 \n"      \
        "popl %2 \n"        \
        :"=a"(ret)          \
        :"a"(num), "r"(cnt) \
    );                      \
    ret;                    \
})

int
open (const char *path, int flags, int mode)
{
    return SYSCALL(1, 3);
}
