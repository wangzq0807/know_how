#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#define SYSCALL(num, cnt)   \
({                          \
    register int ret;       \
    __asm__ (               \
        "int $0x80 \n"      \
        :"=a"(ret)          \
        :"b"(num), "r"(cnt) \
        :"memory"           \
    );                      \
    ret;                    \
})

int exit(int code);

#define fork()      SYSCALL(2, 0)

int read(int fd, void *buf, int count);
int write(int fd, const void *buf, int count);
int open(const char *path, int flags, int mode);
int close(int fd);

#endif // __SYSCALLS_H__
