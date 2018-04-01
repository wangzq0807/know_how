#include "arch.h"
#include "asm.h"

static void
init_8259A()
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
init_idt()
{
}
static void
init_gdt()
{
}

void
init_regs()
{
    init_8259A();
    init_idt();
    init_gdt();
}

