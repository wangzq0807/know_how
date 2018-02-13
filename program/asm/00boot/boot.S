/* 系统启动时，会将软盘第一个扇区（512字节）加载到0x7c00位置
 * 然后跳转到0x7c00位置
 */
BOOTSEG		= 0x07C0
.code16

/*代码段*/
.text
/* 指定程序入口，保存在elf格式中，链接时会用到
 * 若不指定，默认为代码段开始位置
 * 由于我们不使用elf格式，所以不指定也OK
 */
.global _start
_start:
	jmpl $BOOTSEG, $_start2	/* 长跳转 `$代码段,$偏移量`；跳转后改变cs值为0x7c0 */
							/* 直接以$_start2作为偏移量是不严谨的，更一般的做法是$(_start2 - _start) */
							/* 在Makefile中指定了`-Ttext 0x00`也就是说_start的值为0，*/
							/* 因此$(_start2 - _start) == $_start2*/
_start2:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7c00, %sp
	sti						/* 允许中断 */
	cld						/* 清rep指令方向，使源为ds:si,目的: es:di */
	/*********************
	 * BIOS 功能 10H / AH=13H
	 * 功能描述：在Teletype模式下显示字符串
	 * 入口参数：AH＝13H
	 * AL＝显示输出方式
	 * BH＝页码
	 * BL＝属性(若AL=00H或01H)
	 * CX＝显示字符串长度
	 * (DH、DL)＝坐标(行、列)
	 * ES:BP＝显示字符串的地址
	 *********************/
	 movw $0x1301, %ax		/* 功能号13, 显示属性在BL中，光标改变*/
	 movw $07, %bx			/* 第1页 */
	 movw $12, %cx			/* 12个字符 */
	 movw $0x1208, %dx		/* 19行，9列 */
	 movw $_msg, %bp
	 int  $0x10
_loop:
	 jmp _loop
_msg:
	.ascii "Hello,World!"

	.org 510				/*将下一条语句放置到代码段510的位置*/
	.word 0xAA55			/*软盘启动标志*/

