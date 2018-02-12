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
	movw %ax, %es
    lgdt gdt_ptr
	call setup_idt
	/* 系统调用 */
	movl $idt_tab, %ebx			/*idt 基地址 */
	movl $0x80, %eax			/* 变址 */
	leal 0(%ebx, %eax, 8), %esi	/* 使si指向陷阱门 */
	movw $write_char, 0(%si)	/* 陷阱门:偏移地址 */
	movw $0x08, 2(%si)			/* 陷阱门:代码段选择符 */
	movl $0xef00, 4(%si)		/* 陷阱门:特权3 */

	/* 移动到用户态 */
	movw $0x28, %ax
	ltr %ax
    lss tss1_stack0, %esp       /* 设置ss和esp */
	sti
    pushl $(0x20 + 0x3)         /* ss, gdt第4项,task1数据段,特权级3 */
    pushl $tss1_stack3
    pushf						/* eflags */
    pushl $(0x18 + 0x3)         /* cs, gdt第3项,tash1代码段,特权级3 */
    pushl $task1				/* ip */
    iret

task1:
	movw $0x23, %ax
	movw %ax, %ds				/* 设置数据段 */
    pushl $0
	pushl $0
	call start_main
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
    iret

/* 全局描述符表 */
gdt_ptr: .word 0x7ff			/* 2^11 = 2048, 256项 */
	.long gdt_table	            /* base = gdt_table */

gdt_table: .word 0, 0, 0, 0
	/* 代码段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0x9a00			/* 1001 : 段存在,特权00，系统段；1011:代码段，非一致，可读可执行，已访问  */
	.word 0x00C0			/* 粒度4K */
	/* 数据段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0x9200			/* 数据段 */
	.word 0x00C0			/* 粒度4K */
	/* task1代码段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0xfa00			/* 1111 : 段存在,特权3，系统段；1011:代码段，非一致，可读可执行，已访问  */
	.word 0x00C0			/* 粒度4K */
	/* task1数据段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0xf200			/* 数据段 */
	.word 0x00C0			/* 粒度4K */
	/* task1任务描述符 */
	.word 103
	.word tss1
	.word 0xe900
	.word 0

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
    lea ignore_interactor, %bx
    movl $256, %ecx
    lea  idt_tab, %edx
_rep_set:
    movl %ebx, (%edx)
    movl %eax, 4(%edx)
    addl $8, %edx
    dec %ecx
    jne _rep_set                    # ZF标志为0则跳转
	lidt idt_ptr
    ret
ignore_interactor:
    pushl %eax
    popl %eax
    iret
/* 一个空的tss段 */
tss1:
	.long 0						/* 上一个任务 */
	.long tss1_stack0, 0x10		/* sp0, ss0 */
	.long 0			, 0
	.long 0			, 0
	.long 0			, 0			/* cr3, ip */
	.long 0						/* eflags */
	.long 0, 0, 0, 0			/* eax, ecx, edx, ebx */
	.long 0, 0, 0, 0			/* esp, ebp, esi, edi */
	.long 0, 0, 0, 0 			/* es, cs, ss, ds */
	.long 0, 0, 0, 0			/* fs, gs, ldt, iomap */

/* 堆栈 */
    .fill 256, 4, 0          /* 堆栈数据 */
tss1_stack0:
    .long tss1_stack0        /* 偏移地址 */
	.word 0x10

    .fill 256, 4, 0         /* 堆栈数据 */
tss1_stack3:
    .long tss1_stack3       /* 偏移地址 */
	.word 0x23

/* 当前光标位置 */
_CUR_POS:
    .long 0

loop1:
    jmp loop1


_end:
