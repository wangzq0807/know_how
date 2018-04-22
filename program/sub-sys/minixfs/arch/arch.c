#include "arch.h"
#include "asm.h"
#include "log.h"

/* 中断描述符 */
struct X86Desc idt_table[256] = { 0 };
struct X86DTR idt_ptr = { 0 };
/* 全局描述符 */
struct X86Desc gdt_table[5] = { 0 };
struct X86DTR gdt_ptr = { 0 };
/* 局部描述符 */
struct X86Desc ldt_table_1[2] = { 0 };
struct X86DTR ldt_ptr_1 = { 0 };
struct X86Desc ldt_table_2[2] = { 0 };
struct X86DTR ldt_ptr_2 = { 0 };

extern void on_timer_intr();
extern void on_ignore_intr();

/* 中断门: 中断正在处理时,IF清0,从而屏蔽其他中断 */
void
set_intr_gate(int32_t num, void *func_addr)
{
    const uint32_t selector = KNL_CS;
    const uint32_t offset = (uint32_t)func_addr;
    idt_table[num].d_low = (selector << 16) | (offset & 0xFFFF);
    idt_table[num].d_high = (offset & 0xFFFF0000) | GATE_INTR_FLAG;
}

/* 中断门: 中断正在处理时,IF不清零,可响应更高优先级的中断 */
void
set_trap_gate(int32_t num, void *func_addr)
{
    const uint32_t selector = KNL_CS;
    const uint32_t offset = (uint32_t)func_addr;
    idt_table[num].d_low = (selector << 16) | (offset & 0xFFFF);
    idt_table[num].d_high = (offset & 0xFFFF0000) | GATE_TRAP_FLAG;
}

static void
_init_8259A()
{
    /*init_ICW1: 00010001b,多片级联,使用ICW4*/
    outb(0x11, 0x20);
    outb(0x11, 0xA0);
    /* init_ICW2: 中断号 0x20 - 0x27, 0x28 - 0x2f */
    outb(0x20, 0x21);
    outb(0x28, 0xA1);
    /* init_ICW3: 100b, IR2 接从片 */
    outb(0x4, 0x21);        /* 100b, IR2 接从片 */
    outb(0x2, 0xA1);        /* 接主片IR2 */
    /* init_ICW4: 普通全嵌套, 非缓冲, 非自动结束 */
    outb(0x1, 0x21);        /* 普通全嵌套, 非缓冲, 非自动结束 */
    outb(0x1, 0xA1);
}

static void 
_init_timer()
{
    /* 写入控制字, 使用方式3, 计数器0 */
    outb(0x36, 0x43);
    /* 写入计数值，设置每10ms中断一次 */
    uint8_t low = BYTE1(11930);
    outb(low, 0x40);    /* 先写低字节 */
    uint8_t high = BYTE2(11930);
    outb(high, 0x40);   /* 后写高字节 */
}

void
on_timer_handler()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
    outb(0x20, 0xA0);
    print("A");
    print("B");
    print("C");
    print("D");
}

void
on_ignore_handler()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
    outb(0x20, 0xA0);
}

static void
setup_idt()
{
    /* 设置默认中断 */
    for (int32_t i = 0; i < 256; ++i) {
        set_intr_gate(i, &on_ignore_intr);
    }
    /* 设置时钟中断 */
    set_intr_gate(INTR_TIMER, &on_timer_intr);
    /* 重新加载idt */
    const uint32_t base_addr = (uint32_t)(&idt_table);
    idt_ptr.r_limit = 256*8 -1;
    idt_ptr.r_addr = base_addr;
    lidt(&idt_ptr);
}

static struct X86Desc
_setup_seg_dt(uint32_t base, uint32_t limit, uint8_t type, uint8_t dpl, uint8_t prop)
{
    const uint32_t SEG_LIMIT_16 = limit & 0xFFFF;        // 取limit的0-15位
    const uint32_t SEG_LIMIT_20 = limit & 0xF0000;       // 取limit的19-16位
    const uint32_t SEG_BASE_16 = base & 0xFFFF;          // 取base的0-15位
    const uint32_t SEG_BASE_24 = base & 0xFF0000;        // 取base的23-16位
    const uint32_t SEG_BASE_32 = base & 0xFF000000;      // 取base的31-24位

    const uint32_t SEG_TYPE = (type & 0xF) << 8;         // 类型
    const uint32_t SEG_DPL = (dpl & 0xF) << 12;          // 特权级0
    const uint32_t SEG_PROP = (prop & 0xF) << 20;        // 粒度,操作数,等

    struct X86Desc seg_dt = { 0 };
    seg_dt.d_low = SEG_BASE_16 << 16 | SEG_LIMIT_16;
    seg_dt.d_high = SEG_BASE_32 | SEG_PROP | SEG_LIMIT_20 |
                    SEG_DPL | SEG_TYPE | (SEG_BASE_24 >> 16);
    return seg_dt;
}
static struct X86Desc
_setup_code_dt(uint32_t base, uint32_t limit)
{
    const uint8_t CS_TYPE = 0xA;        // 非一致，可读
    const uint8_t CS_DPL  = 0x9;        // 存在，特权级0
    const uint8_t CS_PROP = 0xC;        // 粒度4KB，32位操作数
    return _setup_seg_dt(base, limit, CS_TYPE, CS_DPL, CS_PROP);
}
static struct X86Desc
_setup_data_dt(uint32_t base, uint32_t limit)
{
    const uint32_t DS_TYPE = 0x2;       // 非一致，可读
    const uint32_t DS_DPL  = 0x9;       // 存在，特权级0
    const uint32_t DS_PROP = 0xC;       // 粒度4KB，32位操作数
    return _setup_seg_dt(base, limit, DS_TYPE, DS_DPL, DS_PROP);
}

static void
setup_gdt()
{
    gdt_table[1] = _setup_code_dt(0, 0xFFFFF);
    gdt_table[2] = _setup_data_dt(0, 0xFFFFF);
    /* 重新加载gdt */
    const uint32_t base_addr = (uint32_t)(&gdt_table);
    gdt_ptr.r_limit = 5*8 -1;
    gdt_ptr.r_addr = base_addr;
    lgdt(&gdt_ptr);
}

void
init_regs()
{
    cli();
    setup_idt();
    setup_gdt();

    _init_8259A();
    _init_timer();

    sti();
}

