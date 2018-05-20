// NOTE:中断处理前要保存寄存器的值
.text
.global on_timer_intr
on_timer_intr:
    pushal
    pushl %ds
    call on_timer_handler
    popl %ds
    popal
    iret

.global on_ignore_intr
on_ignore_intr:
    pushal
    pushl %ds
    call on_ignore_handler
    popl %ds
    popal
    iret

.global on_syscall_intr
on_syscall_intr:
    pushal
    pushl %ds
    call on_syscall_handler
    popl %ds
    popal
    iret
