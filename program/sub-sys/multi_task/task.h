#ifndef __MAIN_H__
#define __MAIN_H__
#include "defs.h"
/* 内核代码段和数据段 */
#define KNL_CS      0x8
#define KNL_DS      0x10
/* 门描述符标志 */
#define GATE_INTR_FLAG  0x8E00
#define GATE_TRAP_FLAG  0x8F00
/* 中断向量号 */
#define INTR_HW_BEG     0x20            // 硬件中断号开始
#define INTR_TIMER      INTR_HW_BEG

struct X86Desc {
    uint32_t d_low;
    uint32_t d_high;
};

struct X86DTR {
    uint16_t r_limit;
    uint32_t r_addr;
} __attribute__((packed));  // 取消对齐优化

struct X86TSS {
    uint16_t    t_pre_TSS;

    uint32_t    t_ESP_0;
    uint16_t    t_SS_0;
    uint32_t    t_ESP_1;
    uint16_t    t_SS_1;
    uint32_t    t_ESP_2;
    uint16_t    t_SS_2;

    uint32_t    t_CR3;
    uint32_t    t_EIP;
    uint32_t    t_EFLAGS;
    uint32_t    t_EAX;
    uint32_t    t_ECX;
    uint32_t    t_EDX;
    uint32_t    t_EBX;
    uint32_t    t_ESP;
    uint32_t    t_EBP;
    uint32_t    t_ESI;
    uint32_t    t_EDI;

    uint32_t    t_ES;
    uint32_t    t_CS;
    uint32_t    t_SS;
    uint32_t    t_DS;
    uint32_t    t_FS;
    uint32_t    t_GS;
    uint32_t    t_LDT;
    uint16_t    t_Trap;
    uint16_t    t_IO_map;
};

#endif // __ARCH_H__