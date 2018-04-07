#include "arch.h"
#include "asm.h"
#include "log.h"

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

static void 
_on_timer()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
    const char msg[] = "T";
    print(msg);
}

static void
_on_ignore()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
}

uint64_t idt_table[512] = { 0 };

static inline void 
reg_intr_gate(uint64_t *idt, int32_t num, void *func_addr) {
    const uint32_t selector = TSK_CS;
    const uint32_t offset = (uint32_t)func_addr;
    idt_table[(num << 1)] = (selector << 16) | (offset & 0xFFFF);
    idt_table[(num << 1) + 1] = (offset & 0xFFFF0000) | INTR_GATE_FLAG;
}

static void
setup_idt()
{
    /* 设置默认中断 */
    for (int32_t i = 0; i < 256; ++i) {
        reg_intr_gate(idt_table, i, &_on_ignore);
    }
    /* 设置时钟中断 */
    reg_intr_gate(idt_table, 0, &_on_timer);
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

    // sti();
}

