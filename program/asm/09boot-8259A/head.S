.code32
/****************************************
 * 0xB8000-0xBFFFF 共32KB内存空间为显存
 * 显示缓冲区分为8个页，每页4k
 * 每页25行，每行160B；
 * 每行80个字符，每个字符占两个字节
 * 7　　| 6　　5　　4　|　3　|　2　　1　　0 |
 * 闪烁 | rgb(背景色) | 高亮 | rgb（前景色) |
 *****************************************/
SCREEN_ADR = 0xB8000    /* 显存地址 */
ONE_LINE   = 160        /* 一行的空间大小 */
ONE_PAGE   = 0x1000     /* 一页的空间大小 */
CHAR_PROP  = 0x07       /* 字符属性 */

.text
.global _start
_start:
    movl $0x10, %eax
    movw %ax, %ds
    lss init_stack, %esp        # 设置ss和esp
    call init_8259A             /* 初始化8259A */
    call setup_idt              # 初始化中断描述符表
    lidt idt_ptr                # 加载中断描述符表
    sti                         # 允许中断
    call setup_timer

    movl $0x41, %eax            # 大写字母: A
    call write_char
    movl $0x42, %eax
    call write_char
    jmp loop1
write_char:
    pushl %ebx
    pushl %ecx
    movl _CUR_POS, %ebx         # 取光标位置
    pushl %ebx
    movl $SCREEN_ADR, %ecx      # 取显存基地
    addl %ecx, %ebx             # 计算字符显示地址 = 显存基地址 + 光标位置
    movb $CHAR_PROP, %ah        # 设置字符属性
    movw %ax, (%ebx)            # 写入显存
    /* 保存光标的下一个位置 */
    popl %ebx
    addl $2, %ebx
    cmpl $0x2000, %ebx
    jb _continue                # ebx 小于0x2000，则跳转
    xorl %ebx, %ebx             # 否则，清零
_continue:
    movl %ebx, _CUR_POS
    popl %ecx
    popl %ebx
    ret

/* 中断描述符表 */
.align 4
idt_ptr:
    .word 256*8 - 1         # IDT限长
	.word idt_tab, 0        # 基地址
.align 8
idt_tab: .fill 256, 8, 0    # 中断描述符表

setup_idt:
    movl $0x00008e00, %eax
    movl $0x00080000, %ebx
    lea timer_interrupt, %bx
    movl $256, %ecx
    lea  idt_tab, %edx
_rep_set:
    movl %ebx, (%edx)
    movl %eax, 4(%edx)
    addl $8, %edx
    dec %ecx
    jne _rep_set                    # ZF标志为0则跳转
    ret
setup_timer:
	movb $0x36, %al			# 8253控制字, 方式3，计数器0
	movl $0x43, %edx		# 写入
	outb %al, %dx
	movl $119300, %eax      # 8253计数初值,每0.1s触发一次,8253频率1193000HZ
	movl $0x40, %edx		# 写入计数器0
	outb %al, %dx
	movb %ah, %al
	outb %al, %dx
    ret

timer_interrupt:
	push %ds
    pushl %eax
	movl $0x10, %eax
	mov  %ax, %ds
	movl $0x20, %eax		# 设置8259A的OCW2,发送结束中断命令
	outb %al, $0x20
    movl $0x43, %eax
    call write_char
    popl %eax
	pop %ds
    iret

init_8259A:
    call _init_ICW1
    call _init_ICW2
    call _init_ICW3
    call _init_ICW4
    ret

_init_ICW1:
    movb $0x11, %al         /*00010001b,多片级联,使用ICW4*/
    outb %al, $0x20         /* 主片 */
    outb %al, $0xA0         /* 从片 */
    ret
_init_ICW2:
    movb $0x20, %al         /* 中断号 0x20 - 0x27 */
    outb %al, $0x21         /* 主片 */
    movb $0x28, %al         /* 中断号 0x28 - 0x2f */
    outb %al, $0xA1         /* 从片 */
    ret
_init_ICW3:
    movb $0x4, %al          /* 100b, IR2 接从片 */
    outb %al, $0x21         /* 主片 */
    movb $0x2, %al          /* 接主片IR2 */
    outb %al, $0xA1         /* 从片 */
    ret
_init_ICW4:
    movb $0x1, %al          /* 普通全嵌套, 非缓冲, 非自动结束 */
    outb %al, $0x21
    outb %al, $0xA1
    ret


/* 当前光标位置 */
_CUR_POS:
    .long 0

/* 堆栈 */
    .fill 256, 4, 0         # 堆栈数据
init_stack:
    .long init_stack        # 偏移地址
    .word 0x10              # 数据段选择符

loop1:
    jmp loop1

.org 8190
    .word 0xAA55
_end:
