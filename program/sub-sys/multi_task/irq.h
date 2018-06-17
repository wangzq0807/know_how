#ifndef __IRQ_H__
#define __IRQ_H__

#include "x86.h"

/* 中断向量号 */
#define IRQ_HW_BEG      0x20            // 硬件中断号开始
#define IRQ_TIME        IRQ_HW_BEG
#define IRQ_SYSCALL     0x80
#define IRQ_IGNORE      0xff

#ifndef __INTR_S__
struct X86Desc;
extern struct X86Desc idt_table[256];

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