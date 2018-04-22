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
#define GATE_INTR_FLAG  0x8E00
#define GATE_TRAP_FLAG  0x8F00
/* 中断向量号 */
#define INTR_HW_BEG     0x20            // 硬件中断号开始
#define INTR_TIMER      INTR_HW_BEG
#define INTR_KEYBOARD   INTR_HW_BEG + 1
#define INTR_SLAVE      INTR_HW_BEG + 2
#define INTR_SERIAL2    INTR_HW_BEG + 3
#define INTR_SERIAL1    INTR_HW_BEG + 4
#define INTR_LPT        INTR_HW_BEG + 5
#define INTR_FLOPPY     INTR_HW_BEG + 6
#define INTR_RELTIME    INTR_HW_BEG + 7
#define INTR_PS2        INTR_HW_BEG + 12
#define INTR_DISK       INTR_HW_BEG + 14

struct X86Desc {
    uint32_t d_low;
    uint32_t d_high;
};

struct X86DTR {
    uint16_t r_limit;
    uint32_t r_addr;
} __attribute__((packed));  // 取消对齐优化


void set_intr_gate(int32_t num, void *func_addr);
void set_trap_gate(int32_t num, void *func_addr);

void init_regs();

#endif // __ARCH_H__