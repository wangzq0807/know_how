#include "arch.h"
#include "asm.h"
#include "log.h"

struct X86Desc idt_table[256] = { 0 };
struct X86IDTR idt_ptr = { 0 };
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
    while(1) {
        pause();
    }
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

    const uint32_t base_addr = (uint32_t)(&idt_table);
    idt_ptr.i_limit = 256*8 -1;
    idt_ptr.i_addr = base_addr;
    lidt(&idt_ptr);
}

static void
setup_gdt()
{
    
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

