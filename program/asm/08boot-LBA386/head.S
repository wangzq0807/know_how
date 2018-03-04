.code32

.text
.global _start
_start:
    movl $0x10, %eax
    movw %ax, %ds
	call setup_idt
/*****************************************************
 * 24-bit LBA方式:
 * 写0x1f1: 0
 * 写0x1f2: 要读的扇区数
 * 写0x1f3: LBA参数的0~7位
 * 写0x1f4: LBA参数的8~15位
 * 写0x1f5: LBA参数的16~23位
 * 写0x1f6: 7~5位,111,第4位0表示主盘,1表示从盘,3~0位,LBA参数的24~27位
 * 写0x1f7: 0x20为读, 0x30为写
 * 读0x1f7: 第4位为0表示读写完成，否则要一直循环等待
 * 读0x1f0: 每次读取1个word,反复循环，直到读完所有数据
 *****************************************************/
read_lba24:
    movw $0x1F1, %dx
    movw $0x0, %ax
    outb %al, %dx       /* 全0 */

    movw $0x1F2, %dx
    movw $0xFF, %ax
    outb %al, %dx       /* 读255个扇区 */

    movw $0x1F3, %dx
    movw $0x2, %ax
    outb %al, %dx       /* 从第2个扇区开始读, LBA 0-7bit */

    movw $0x1F4, %dx
    movw $0x0, %ax
    outb %al, %dx       /* LBA 8-15bit */

    movw $0x1F5, %dx
    movw $0x0, %ax
    outb %al, %dx       /* LBA 16-23bit */

    movw $0x1F6, %dx
    movw $0x0, %ax
    outb %al, %dx       /* master, LBA mode, LBA 24-27 bit */

    movw $0x1F7, %dx
    movw $0x20, %ax
    outb %al, %dx       /* 读操作 */
    xorl %ecx, %ecx
reading:
    movw $0x01F7, %dx   /* 状态 */
    inb %dx, %al
    testb $8, %al       /* 是否读完, 与操作 */
    jz reading

    movw $0xFF, %cx     /* 读255个字 */
    movw $0x10, %eax
    movw %eax, %es      /* 数据段 */
    leal dest_data, %edi  /* dest_data */
    movw $0x1F0, %dx
    rep insw            /* 读取 */

read_ok:
    xorw %ax, %ax
    jmp read_ok

dest_data:
    .fill 512, 1, 1     /* 512个字节 */

/* 中断描述符表 */
.align 4
idt_ptr:
    .word 256*8 - 1         /* IDT限长 */
	.word idt_tab, 0        /* 基地址 */
.align 8
idt_tab: .fill 256, 8, 0    /* 中断描述符表 */

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
    jne _rep_set            /* ZF标志为0则跳转 */
	lidt idt_ptr
    ret
ignore_interrupt:
	movl $0x20, %eax		/* 设置8259A的OCW2,发送结束中断命令 */
	outb %al, $0x20
    iret

loop1:
    jmp loop1

