.code32
/*内核*/
KNL_CODE = 0x08
KNL_DATA = 0x10
/* 任务一 */
TASK1_CODE = 0x18 + 0x03
TASK1_DATA = 0x20 + 0x03
TASK1_TSS = 0x28
/* 任务二 */
TASK2_CODE = 0x30 + 0x03
TASK2_DATA = 0x38 + 0x03
TASK2_TSS = 0x40

.text
.global _start
_start:
    movl $KNL_DATA, %eax
    movw %ax, %ds
    lgdt gdt_ptr
	call setup_idt

	/* 移动到用户态 */
	movw $TASK1_TSS, %ax
	ltr %ax
    lss tss1_stack0, %esp		/* 设置ss和esp */
	sti
    pushl $TASK1_DATA 			/* ss, gdt第4项,task1数据段,特权级3 */
    pushl $tss1_stack3
    pushf						/* eflags */
    pushl $TASK1_CODE			/* cs, gdt第3项,tash1代码段,特权级3 */
    pushl $task1				/* ip */
    iret

task1:
	movw $TASK1_DATA, %ax
	movw %ax, %ds
	movl $0xff, %ecx
1:
	loop 1b
    jmp task1

task2:
	movw $TASK2_DATA, %ax
	movw %ax, %ds
	pushl %ebx
1:
	loop 1b
    jmp task2

/* 全局描述符表 */
gdt_ptr: .word 0x7ff			/* 2^11 = 2048, 256项 */
	.long gdt_table	            /* base = gdt_table */

.align 8
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
	/* task2代码段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0xfa00			/* 1111 : 段存在,特权3，系统段；1011:代码段，非一致，可读可执行，已访问  */
	.word 0x00C0			/* 粒度4K */
	/* task2数据段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 0x0000 */
	.word 0xf200			/* 数据段 */
	.word 0x00C0			/* 粒度4K */
	/* task2任务描述符 */
	.word 103
	.word tss2
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
    lea ignore_interrupt, %bx
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
ignore_interrupt:
    pushl %eax
	ljmp $TASK2_TSS, $0
    popl %eax
    iret
/* 任务一tss段,空的 */
.align 4
tss1:
	.long 0							/* 上一个任务 */
	.long tss1_stack0, KNL_DATA		/* sp0, ss0 */
	.long 0			, 0
	.long 0			, 0
	.long 0			, 0			/* cr3, ip */
	.long 0						/* eflags */
	.long 0, 0, 0, 0			/* eax, ecx, edx, ebx */
	.long 0, 0, 0, 0			/* esp, ebp, esi, edi */
	.long 0, 0, 0, 0 			/* es, cs, ss, ds */
	.long 0, 0, 0, 0			/* fs, gs, ldt, iomap */

/* 任务一堆栈 */
.align 4
    .fill 256, 4, 0          /* 堆栈数据 */
tss1_stack0:
    .long tss1_stack0        /* 偏移地址 */
	.word KNL_DATA

.align 4
    .fill 256, 4, 0         /* 堆栈数据 */
tss1_stack3:
    .long tss1_stack3       /* 偏移地址 */
	.word TASK1_DATA

/* 任务二tss段 */
.align 4
tss2:
	.long 0							/* 上一个任务 */
	.long tss2_stack0, KNL_DATA		/* sp0, ss0 */
	.long 0			, 0
	.long 0			, 0
	.long 0			, task2		/* cr3, ip */
	.long 0x200					/* eflags */
	.long 0, 0, 0, 0			/* eax, ecx, edx, ebx */
	.long tss2_stack3, 0 		/* esp, ebp */
	.long 0, 0						/* esi, edi */
	.long TASK2_DATA, TASK2_CODE /* es, cs */
	.long TASK2_DATA, TASK2_DATA /* ss, ds */
	.long TASK2_DATA, TASK2_DATA /* fs, gs */
	.long 0, 0		/* ldt, iomap */

/* 任务二堆栈 */
.align 4
    .fill 256, 4, 0          /* 堆栈数据 */
tss2_stack0:
    .long tss2_stack0        /* 偏移地址 */
	.word KNL_DATA

.align 4
    .fill 256, 4, 0         /* 堆栈数据 */
tss2_stack3:
    .long tss2_stack3       /* 偏移地址 */
	.word TASK2_DATA


loop1:
    jmp loop1


_end:
