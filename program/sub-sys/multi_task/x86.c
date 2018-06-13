#include "x86.h"
#include "asm.h"

/* 全局描述符 */
struct X86Desc gdt_table[7] = { 0 };
struct X86DTR gdt_ptr = { 0 };
/* 中断描述符 */
struct X86Desc idt_table[256] = { 0 };
struct X86DTR idt_ptr = { 0 };
/* 当前tss */
uint32_t current_tss = 1;

// 任务状态段DPL : 存在，特权级0，系统段
#define TSS_DPL     0x8
// LDT段DPL : 存在，特权级0，系统段
#define LDT_DPL     0x8
// 限长
#define LDT_LIMIT   (3*8)
#define TSS_LIMIT   103

extern void on_timer_intr();
extern void on_ignore_intr();
extern void on_syscall_intr();

void
set_sys_call() {
    set_trap_gate(0x80, on_syscall_intr, 3);
}

void
setup_segment_desc(struct X86Desc* desc, uint32_t base, uint32_t limit, uint8_t type, uint8_t dpl, uint8_t prop)
{
    const uint32_t SEG_LIMIT_16 = limit & 0xFFFF;        // 取limit的0-15位
    const uint32_t SEG_LIMIT_20 = limit & 0xF0000;       // 取limit的19-16位
    const uint32_t SEG_BASE_16 = base & 0xFFFF;          // 取base的0-15位
    const uint32_t SEG_BASE_24 = base & 0xFF0000;        // 取base的23-16位
    const uint32_t SEG_BASE_32 = base & 0xFF000000;      // 取base的31-24位

    const uint32_t SEG_TYPE = (type & 0xF) << 8;         // 类型
    const uint32_t SEG_DPL  = (dpl & 0xF) << 12;         // 特权级0
    const uint32_t SEG_PROP = (prop & 0xF) << 20;        // 粒度,操作数位数,等

    desc->d_low = SEG_BASE_16 << 16 | SEG_LIMIT_16;
    desc->d_high = SEG_BASE_32 | SEG_PROP | SEG_LIMIT_20 |
                    SEG_DPL | SEG_TYPE | (SEG_BASE_24 >> 16);
}

void
setup_gdt()
{
    const uint8_t KNL_DPL = 0x9;        // 内核段DPL : 存在，特权级0，数据段/代码段
    setup_code_desc(&gdt_table[KNL_CS>>3], 0, 0xFFFFF, KNL_DPL);
    setup_data_desc(&gdt_table[KNL_DS>>3], 0, 0xFFFFF, KNL_DPL);

    /* 重新加载gdt */
    const uint32_t base_addr = (uint32_t)(&gdt_table);
    gdt_ptr.r_limit = 7*8 -1;
    gdt_ptr.r_addr = base_addr;
    lgdt(&gdt_ptr);
    reload_sregs(KNL_CS, KNL_DS);
}

void
setup_idt()
{
    /* 设置默认中断 */
    for (int32_t i = 0; i < 256; ++i) {
        set_intr_gate(i, &on_ignore_intr, 0);
    }
    /* 设置时钟中断 */
    set_intr_gate(INTR_TIMER, &on_timer_intr, 0);
    /* 系统调用 */
    set_sys_call();
    /* 重新加载idt */
    const uint32_t base_addr = (uint32_t)(&idt_table);
    idt_ptr.r_limit = 256*8 -1;
    idt_ptr.r_addr = base_addr;
    lidt(&idt_ptr);
}

void
switch_tss(struct X86TSS *tss, struct X86Desc *ldt)
{
    tss->t_LDT = KNL_LDT;
    if (current_tss == 1) {
        setup_tss_desc(&gdt_table[KNL_TSS2>>3], (uint32_t)tss, TSS_LIMIT, TSS_DPL);
        current_tss = 2;

        // TO:我想用 KNL_TSS2 替代0x20，但不知道该怎么做
        __asm__ volatile (
            "ljmp $0x20, $0 \n"
        );
    }
    else {
        setup_tss_desc(&gdt_table[KNL_TSS1>>3], (uint32_t)tss, TSS_LIMIT, TSS_DPL);
        current_tss = 1;
        __asm__ volatile (
            "ljmp $0x18, $0 \n"
        );
    }
}

void
start_first_task(struct X86TSS *tss, struct X86Desc *ldt, void *stack, void *func)
{
    tss->t_LDT = KNL_LDT;
    setup_tss_desc(&gdt_table[KNL_TSS1>>3], (uint32_t)tss, TSS_LIMIT, TSS_DPL);
    setup_ldt_desc(&gdt_table[KNL_LDT>>3], (uint32_t)ldt, LDT_LIMIT, LDT_DPL);
    /* 跳转到用户空间:任务一 */
    ltr(KNL_TSS1);
    lldt(KNL_LDT);
    sti();
    switch_to_user(USR_CS, USR_DS, stack, func);
}