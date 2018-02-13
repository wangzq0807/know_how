/* 系统启动时，会将软盘第一个扇区（512字节）加载到0x7c00位置
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
_load:
	/************************
	 * BIOS 功能 13H / AH=02H 
	 * AL = 要读取的扇区数
	 * CH = 柱面号 (0...79)
	 * CL = 扇区号 (1...18)
	 * DH = 磁头号 (0...1)
	 * DL = drive number
	 * ES:BX 数据读取到的位置
	 ************************/
	movw $(0x0200+HEADLEN), %ax		/* 读16个扇区,共8KB */
	movw $0x0002, %cx				/* 从扇区2开始读 */
	movw $0x0000, %dx				/* 磁盘第一面 */
	movw $HEADSEG, %bx
	movw %bx, %es					/* 存放到0x10000 */
	xorw %bx, %bx
	int $0x13
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
	movw $0x1, %ax
	lmsw %ax
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

	.org 510				/*将下一条语句放置到代码段510的位置*/
	.word 0xAA55			/*软盘启动标志*/

