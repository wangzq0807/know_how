#ifndef __STDIO_H__
#define __STDIO_H__

#include "sys/syscalls.h"

#define O_RDONLY    0x1
#define O_WRONLY    0x2
#define O_NONBLOCK  0x4
#define O_APPEND    0x8

#define O_CREAT     0x200
#define O_TRUNC     0x400

#endif
