#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#define SYSCALL0(num)   \
({                          \
    register int ret;       \
    __asm__ volatile (      \
        "int $0x80 \n"      \
        :"=a"(ret)          \
        :"a"(num)           \
        :"memory"           \
    );                      \
    ret;                    \
})

#define SYSCALL1(num, p1)   \
({                          \
    register int ret;       \
    __asm__ volatile (      \
        "pushl %2 \n"       \
        "int $0x80 \n"      \
        "addl $4, %%esp \n" \
        :"=a"(ret)          \
        :"a"(num), "r"(p1)  \
        :"memory"           \
    );                      \
    ret;                    \
})

#define SYSCALL2(num, p1, p2)   \
({                          \
    register int ret;       \
    __asm__ volatile (      \
        "pushl %2 \n"       \
        "pushl %3 \n"       \
        "int $0x80 \n"      \
        "addl $8, %%esp \n" \
        :"=a"(ret)          \
        :"a"(num), "r"(p2), "r"(p1) \
        :"memory"           \
    );                      \
    ret;                    \
})

#define SYSCALL3(num, p1, p2, p3)   \
({                          \
    register int ret;       \
    __asm__ volatile (      \
        "pushl %2 \n"       \
        "pushl %3 \n"       \
        "pushl %4 \n"       \
        "int $0x80 \n"      \
        "addl $12, %%esp \n" \
        :"=a"(ret)          \
        :"a"(num), "r"(p3), "r"(p2), "r"(p1) \
        :"memory"           \
    );                      \
    ret;                    \
})

#define exit(code)  SYSCALL1(1, code)
#define fork()      SYSCALL0(2)
#define read(fd, buf, count)    SYSCALL3(3, fd, buf, count)
#define write(fd, buf, count)   SYSCALL3(4, fd, buf, count)
#define open(path, flags, mode) SYSCALL3(5, path, flags, mode)
#define close(fd)   SYSCALL1(6, fd)

#endif // __SYSCALLS_H__
