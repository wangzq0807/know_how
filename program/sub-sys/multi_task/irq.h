#ifndef __IRQ_H__
#define __IRQ_H__

#include "x86.h"

/* 中断向量号 */
#define IRQ_PAGE        14              // 页故障

#define IRQ_HW_BEG      0x20            // 硬件中断号开始
#define IRQ_TIME        IRQ_HW_BEG
#define IRQ_SYSCALL     0x80
#define IRQ_IGNORE      0xff

#ifndef __INTR_S__
struct X86Desc;
extern struct X86Desc idt_table[256];

struct TrapFrame {
    /* 由硬件入栈 */
    int     tf_EIP;
    int     tf_CS;
    int     tf_EFLAGS;
    /* 系统特权级改变时入栈 */
    int     tf_ESP;
    int     tf_SS;
};

struct ErrorFrame {
    /* 当段错误时由硬件入栈(8-14) */
    int     ef_errno;
    /* 由硬件入栈 */
    int     ef_EIP;
    int     ef_CS;
    int     ef_EFLAGS;
    /* 系统特权级改变时入栈 */
    int     ef_ESP;
    int     ef_SS;
};

struct IrqFrame {
    int     if_DS;
    /* 下面是pushal保存的寄存器 */
    int     if_EDI;
    int     if_ESI;
    int     if_EBP;
    int     if_OSP;
    int     if_EBX;
    int     if_EDX;
    int     if_ECX;
    int     if_EAX;
    /* 中断号 */
    int     if_irqno;
    /* 由硬件入栈 */
    union {
        struct TrapFrame    if_trap;
        struct ErrorFrame   if_error;
    };
};

/**********
 * 系统门
 **********/


void
setup_idt();

void
on_timer_handler();

void
on_ignore_handler();

void
on_syscall_handler();
#endif  // __INTR_S__

#endif // __IRQ_H__