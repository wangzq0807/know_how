#ifndef __INTR_S__
#define __INTR_S__
#include "irq.h"

.text
#define TRAP_FUNC(num)  \
.global trap##num;      \
trap##num:              \
    pushl $num;         \
    jmp all_intr_request

TRAP_FUNC(IRQ_TIME)
TRAP_FUNC(IRQ_IGNORE)
TRAP_FUNC(IRQ_SYSCALL)

/* NOTE:中断处理前要保存寄存器的值 */
all_intr_request:
    pushal
    pushl %ds
    call on_all_irq
    popl %ds
    popal
    addl $4, %esp    /* 中断号出栈 */
    iret

#endif  /* __INTR_S__ */
