/* 系统启动时，会将软盘或硬盘第一个启动扇区（512字节）加载到0x7c00位置
 * 然后跳转到0x7c00位置
 */
BOOTSEG		= 0x07C0
HEADSEG		= 0x1000
HEADLEN		= 16
.code16

/*代码段*/
.text
.global _start
_start:
	jmpl $BOOTSEG, $_start2
_start2:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7c00, %sp
	sti						/* 允许中断 */
test_lba:
	/*********************
	 * INT13h Extensions
	 * 测试是否支持 LBA
	 *********************/
	movw $0x4100, %ax	/* AH = 0x41 */
	movw $0x55AA, %bx	/* BX = 0x55AA */
	movw $0x80, %cx		/* CL = 0x80 */
	int  $0x13
	jc lba_error
	jmp lba_read

lba_error:
	xorw %ax, %ax
	jmp lba_error

lba_pack:
	.word 0x1000	/* 16 byte */
	.word HEADLEN	/* read 16 sector */
	.word 0x0000	/* mem buffer 0x1000:0x000*/
	.word HEADSEG
	.long 0x1		/* start lba address 2 */
	.long 0x0

lba_read:
    /*********************
	 * INT13h Extensions
	 * LBA读磁盘
	 *********************/
	 movw $0x4200, %ax	/* AH = 0x42 */
	 lea lba_pack, %bx	/* DS:SI 指向 lba_pack */
	 movw %bx, %si
	 movw $0x80, %cx	/* CL = 0x80 */
	 int  $0x13
	 jnc read_ok

read_failed:
	xorw %bx, %bx
	jmp read_failed

read_ok:
	/***********************
	 * 将0x10000处代码移动到0x00
	 ***********************/
	cli						/* 关闭中断 */
	cld						/* 清rep指令方向，使源为ds:si,目的: es:di */
	movw $HEADSEG, %ax
	movw %ax, %ds			/* ds = 0x1000 */
	xorw %ax, %ax
	movw %ax, %es			/* es = 0x0000 */
	xorw %si, %si
	xorw %di, %di
	movw $(HEADLEN*256), %cx/* 移动 4K 次 */
	rep movsw 				/* 每次2字节 */

	/* 设置gdtr和idtr */
	movw $BOOTSEG, %ax
	movw %ax, %ds
	lidt idt_ptr
	lgdt gdt_ptr
    /* 开启保护模式 */
    movl %cr0, %eax
    orl  $1, %eax
    movl %eax, %cr0
	/* 设置cs和ip */
	jmpl $8, $0				/* 8为gdt代码段，基地址为0x0000,偏移地址为0x0000 */

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

idt_ptr: .word 0
	.word 0, 0

gdt_ptr: .word 0x7ff			/* 2^11 = 2048, 256项 */
	.word 0x7c00+gdt_table, 0	/* base = 0x7xxx */

