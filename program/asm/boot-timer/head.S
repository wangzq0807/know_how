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
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs

    movl $0x41, %eax            # 大写字母: A
    call write_char
    movl $0x42, %eax
    call write_char
    jmp loop1
write_char:
    movl _CUR_POS, %ebx         # 取光标位置
    pushl %ebx
    movl $SCREEN_ADR, %ecx
    addl %ecx, %ebx             # 计算显存地址 = 显存开始地址 + 光标位置
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
    ret
loop1:
    jmp loop1

/* 当前光标位置 */
_CUR_POS:
    .long 0

.org 8190
    .word 0xAA55
_end:
