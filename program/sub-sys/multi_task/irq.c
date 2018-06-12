#include "irq.h"
#include "task.h"
#include "asm.h"

void
on_timer_handler()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
    outb(0x20, 0xA0);
    switch_task();
}

void
on_ignore_handler()
{
    /* 设置8259A的OCW2,发送结束中断命令 */
    outb(0x20, 0x20);
    outb(0x20, 0xA0);
}

void
on_syscall_handler()
{
    __asm__ volatile (
        "mov $0x10, %%ax \n"
        "mov %%ax, %%ds \n"
        : : : "eax"
    );

    switch_task();
}
