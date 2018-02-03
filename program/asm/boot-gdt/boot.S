/* 系统启动时，会将软盘第一个扇区（512字节）记载到0x7c00位置
 * 然后跳转到0x7c00位置
 */
BOOTSEG		= 0x07C0
.code16

/*代码段*/
.text
/* 
 * 指定程序入口，elf格式会用到
 * 若不指定，默认为代码段开始位置
 */
.global _start
_start:
	jmpl $BOOTSEG, $_start2	/*长跳转 $代码段,$偏移量；跳转后改变cs值为0x7c0*/
_start2:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7c00, %sp
	/* 设置gdtr和idtr */
	lidt idt_ptr
	lgdt gdt_ptr
	/* 开启保护模式 */	
	movw $0x1, %ax
	lmsw %ax
	/* 设置cs和ip */
	jmpl $8, $_loop			/* 8为gdt代码段，基地址为0x7c00,偏移地址为_loop */
_loop:
	 jmp _loop				/* 如果保护模式设置不成功，则无法执行到这条命令 */

gdt_table: .word 0, 0, 0, 0
	/* 代码段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x7c00			/* 基地址 0x7c00 */
	.word 0x9a00			/* 1001 : 段存在,特权00，系统段；1011:代码段，非一致，可读可执行，已访问  */
	.word 0x00C0			/* 粒度4K */
	/* 数据段 */
	.word 0x07FF			/* 段限长2048 x 粒度(4K) = 8M */
	.word 0x7c00			/* 基地址 0x7c00 */
	.word 0x9200			/* 数据段 */
	.word 0x00C0			/* 粒度4K */

idt_ptr: .word 0
	.word 0, 0

gdt_ptr: .word 0x7ff			/* 2^11 = 2048, 256项 */
	.word 0x7c00+gdt_table, 0	/* base = 0x7xxx */

	.org 510				/*将下一条语句放置到代码段510的位置*/
	.word 0xAA55			/*软盘启动标志*/

