#ifndef __X86_H__
#define __X86_H__

#include "defs.h"

/* 内核代码段和数据段 */
#define KNL_CS      0x8
#define KNL_DS      0x10
/* 只使用两个tss段，通过不断反转两个tss实现任务切换 */
#define KNL_TSS1    0x18
#define KNL_TSS2    0x20
/* 所有任务使用相同的LDT描述符 */
#define KNL_LDT     0x28

/* 用户态代码段和数据段 */
#define USR_CS      0xF
#define USR_DS      0x17
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
extern struct X86Desc idt_table[256];

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

/************
 * 描述符
 ************/
void
setup_segment_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t type, uint8_t dpl, uint8_t prop);

/* 代码段描述符：非一致性 */
static inline void
setup_code_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t dpl) {
    const uint8_t CS_TYPE = 0xA;        // 非一致，可读
    const uint8_t CS_PROP = 0xC;        // 粒度4KB，32位操作数
    setup_segment_desc(desc, base, limit, CS_TYPE, dpl, CS_PROP);
}

/* 数据段描述符 */
static inline void
setup_data_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t dpl) {
    const uint32_t DS_TYPE = 0x2;       // 非一致，可写
    const uint32_t DS_PROP = 0xC;       // 粒度4KB，32位操作数
    setup_segment_desc(desc, base, limit, DS_TYPE, dpl, DS_PROP);
}

/* 任务状态段描述符 */
static inline void
setup_tss_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t dpl) {
    const uint32_t TSS_TYPE = 0x9;      // 非忙
    const uint32_t TSS_PROP = 0x0;      // 大于等于104字节
    setup_segment_desc(desc, base, limit, TSS_TYPE, dpl, TSS_PROP);
}

/* 局部描述符 */
static inline void
setup_ldt_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t dpl) {
    const uint32_t LDT_TYPE = 0x2;      // LDT
    const uint32_t LDT_PROP = 0x0;      //
    setup_segment_desc(desc, base, limit, LDT_TYPE, dpl, LDT_PROP);
}

/**********
 * 系统门
 **********/
/* 中断门: 中断正在处理时,IF清0,从而屏蔽其他中断 */
static inline void
set_intr_gate(int32_t num, void *func_addr, int32_t dpl) {
    const uint32_t selector = KNL_CS;
    const uint32_t offset = (uint32_t)func_addr;
    idt_table[num].d_low = (selector << 16) | (offset & 0xFFFF);
    idt_table[num].d_high = (offset & 0xFFFF0000) | GATE_INTR_FLAG | (dpl<<13);
}

/* 陷阱门: 中断正在处理时,IF不清零,可响应更高优先级的中断 */
static inline void
set_trap_gate(int32_t num, void *func_addr, int32_t dpl) {
    const uint32_t selector = KNL_CS;
    const uint32_t offset = (uint32_t)func_addr;
    idt_table[num].d_low = (selector << 16) | (offset & 0xFFFF);
    idt_table[num].d_high = (offset & 0xFFFF0000) | GATE_TRAP_FLAG | (dpl<<13);
}

void
setup_gdt();

void
setup_idt();

void
switch_tss(struct X86TSS *tss, struct X86Desc *ldt);

void
start_first_task(struct X86TSS *tss, struct X86Desc *ldt, void *stack, void *func);

#endif // __X86_H__