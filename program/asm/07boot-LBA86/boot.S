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
	/*********************
	 * 读磁盘驱动器参数
	 * 输出:
	 * dh = 最大磁头号, 15
	 * ch = 最大磁道号的低8位, 19
	 * cl = 每磁道最大扇区数(位0-5) 64，最大磁道号高2位(位6-7)
	 * 总量: 16*20*64*512
	 *********************/
	movw $0x00, %cx	
	movb $0x08, %ah
	int  $0x13
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
	 jc read_failed

read_ok:
	xorw %cx, %cx
	jmp read_ok

read_failed:
	xorw %bx, %bx
	jmp read_failed

lba_pack:
	.word 0x1000	/* 16 byte */
	.word 0xff		/* read count 255(max) section */
	.word 0x200		/* mem buffer 0x1000:0x200*/
	.word 0x1000
	.long 0x802		/* start lba address 2050 */
	.long 0x0


