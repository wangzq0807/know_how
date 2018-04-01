.code32

CODE_SEG = 0x8
DATA_SEG = 0x10

.text
.global _start
_start:
    movl $DATA_SEG, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    lss init_stack, %esp
    lgdt gdt_ptr
    call setup_idt

    call start_main
    /* call start_main */
    jmp loop1

loop1:
    jmp loop1

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
    lea ignore_inter, %bx
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
ignore_inter:
    pushl %eax
    popl %eax
    iret

/* 2KB 堆栈 */
.align  4
    .fill 512, 4, 0
init_stack:
    .long init_stack
    .word DATA_SEG

