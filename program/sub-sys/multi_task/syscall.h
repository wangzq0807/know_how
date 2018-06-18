#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include "defs.h"
#include "irq.h"

pid_t knl_fork(struct IrqFrame *irq);

#endif