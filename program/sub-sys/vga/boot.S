/* 系统启动时，会将软盘第一个扇区（512字节）加载到0x7c00位置
 * 然后执行0x7c00处的指令
 */
BOOT_ADDR   = 0x7C00            /* 第一个扇区对应的内存地址 */
SYS_ADDR    = 0x7C00 + 0x200    /* 第二个扇区对应的内存地址 */
SYS_LEN		= 63
/* 第一个扇区，16位汇编 */
.code16
/*代码段*/
.text
.global _start
_start:
    sti                 /* 开启中断 */
	/************************
	 * BIOS 功能 13H / AH=02H 
	 * AL = 要读取的扇区数
	 * CH = 柱面号 (0...79)
	 * CL = 扇区号 (1...18)
	 * DH = 磁头号 (0...1)
	 * DL = drive number
	 * ES:BX 数据读取到的位置
	 ************************/
	movw $(0x0200+SYS_LEN), %ax		/* 读SYS_LEN个扇区 */
	movw $0x0002, %cx				/* 从扇区2开始读 */
	movw $0x0000, %dx				/* 磁盘第一面 */
    /* 将磁盘数据读取到SYS_ADDR */
	xorw %bx, %bx
    movw %bx, %es
	movw $SYS_ADDR, %bx
	int $0x13

	/***********************
	 *	VESA BIOS Extension	
	 ***********************/
	// 获取控制器信息
	// INT 0x10, AX=0x4F00
	xorw %ax, %ax
	movw %ax, %es
	movw $vbe_info, %di
	movw $0x4F00, %ax
	int $0x10

	/* 字符模式 */
	movw $0x3, %ax
	int $0x10
	
    cli					    /* 关闭中断 */
	/* 设置gdtr和idtr */
	movw $0, %ax
	movw %ax, %ds
	lidt idt_ptr
	lgdt gdt_ptr
	/* 开启保护模式 */
	movw $0x1, %ax
	lmsw %ax
	/* 设置cs和ip */
	jmpl $8, $SYS_ADDR		/* 8代表gdt的第一项，代码段 */

vbe_info:
	.long	0				/* 标志：‘VESA’ */
	.word	0				/* 版本： 0002 */
	.long	0				/* oem地址 */
	.long   0				/* 能力 */
	.long	0				/* 模式地址 */
	.word	0				/* 内存大小: N * 64kb */
	.fill   222, 1, 0		/* 保留 */

gdt_table: .word 0, 0, 0, 0
	/* 代码段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 */
	.word 0x9a00			/* 1001 : 段存在,特权00，系统段；1011:代码段，非一致，可读可执行，已访问  */
	.word 0x00C0			/* 粒度4K */
	/* 数据段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x0000			/* 基地址 */
	.word 0x9200			/* 数据段 */
	.word 0x00C0			/* 粒度4K */

idt_ptr: .word 0
	.word 0, 0

gdt_ptr: .word 0x7ff			/* 2^11 = 2048, 256项 */
	.word gdt_table, 0	        /* base = gdt_table */

    /*将下一条语句放置到代码段510的位置*/
	.org 510				
	.word 0xAA55			/* 可启动标志*/

/* 第二个扇区，32位汇编 */
.code32
CODE_SEG = 0x8
DATA_SEG = 0x10

.text
x32_start:
    movl $DATA_SEG, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    lss init_stack, %esp

    call start_main

loop1:
    jmp loop1

/* 堆栈 */
.align  4
    .fill 256, 4, 0
init_stack:
    .long init_stack
    .word DATA_SEG
