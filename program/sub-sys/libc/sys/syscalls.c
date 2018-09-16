#include "syscalls.h"

int
exit(int code)
{
    return SYSCALL(1, 1);
}

int
read(int fd, void *buf, int count)
{
    return SYSCALL(3, 3);
}

int
write(int fd, const void *buf, int count)
{
    return SYSCALL(4, 3);
}

int
open(const char *path, int flags, int mode)
{
    return SYSCALL(5, 3);
}

int
close(int fd)
{
    return SYSCALL(6, 1);
}
