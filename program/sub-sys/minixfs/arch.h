#ifndef __ARCH_H__
#define __ARCH_H__
#include "defs.h"
/* 内核代码段和数据段 */
#define KNL_CS      0x8
#define KNL_DS      0x10
/*  */
#define TSK_CS      (0x8 + 0b11)
#define TSK_DS      (0x10 + 0b11)
#define TSK_TSS(n)  ((2*(n)+3) << 3)
#define TSK_LDT(n)  (TSK_TSS(n) + 8)
#define TSK_MAX     128
/* 门描述符标志 */
#define INTR_GATE_FLAG  0x8E00
#define TRAP_GATE_FLAG  0x8F00
/* 中断向量号 */
#define INTR_TIMER      0 + 32
#define INTR_KEYBOARD   1 + 32
#define INTR_SLAVE      2 + 32
#define INTR_SERIAL2    3 + 32
#define INTR_SERIAL1    4 + 32
#define INTR_DISK       14 + 32

void init_regs();

#endif // __ARCH_H__