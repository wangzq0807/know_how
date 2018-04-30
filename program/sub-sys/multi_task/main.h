#ifndef __MAIN_H__
#define __MAIN_H__
#include "defs.h"
/* 内核代码段和数据段 */
#define KNL_CS      0x8
#define KNL_DS      0x10
/* 门描述符标志 */
#define GATE_INTR_FLAG  0x8E00
#define GATE_TRAP_FLAG  0x8F00
/* 中断向量号 */
#define INTR_HW_BEG     0x20            // 硬件中断号开始
#define INTR_TIMER      INTR_HW_BEG

struct X86Desc {
    uint32_t d_low;
    uint32_t d_high;
};

struct X86DTR {
    uint16_t r_limit;
    uint32_t r_addr;
} __attribute__((packed));  // 取消对齐优化

struct X86TSS {
    uint16_t    t_pre_TSS;

    uint32_t    t_ESP_0;
    uint16_t    t_SS_0;
    uint32_t    t_ESP_1;
    uint16_t    t_SS_1;
    uint32_t    t_ESP_2;
    uint16_t    t_SS_2;

    uint32_t    t_CR3;
    uint32_t    t_EIP;
    uint32_t    t_EFLAGS;
    uint32_t    t_EAX;
    uint32_t    t_ECX;
    uint32_t    t_EDX;
    uint32_t    t_EBX;
    uint32_t    t_ESP;
    uint32_t    t_EBP;
    uint32_t    t_ESI;
    uint32_t    t_EDI;

    uint32_t    t_ES;
    uint32_t    t_CS;
    uint32_t    t_SS;
    uint32_t    t_DS;
    uint32_t    t_FS;
    uint32_t    t_GS;
    uint32_t    t_LDT;
    uint16_t    t_Trap;
    uint16_t    t_IO_map;
};

#define SCREEN_ADR  0xB8000    /* 显存地址 */
#define ONE_LINE    160        /* 一行的空间大小 */
#define ONE_PAGE    0x1000     /* 一页的空间大小 */
#define CHAR_PROP   0x0F       /* 字符属性(白色) */
uint32_t cursor_pos = 0;
void
print(const char *buffer)
{
    for (uint32_t i = 0; i < 0xffff; ++i) {
        char* next_addr = (char*)(SCREEN_ADR + (cursor_pos << 1));
        if (buffer[i] == '\n') {
            cursor_pos = (cursor_pos + 80) / 80 * 80;
        }
        else if (buffer[i] == '\0') {
            return ;
        }
        else {
            *next_addr = buffer[i];
            *(next_addr+1) = CHAR_PROP;
            ++cursor_pos;
        }
        if (cursor_pos >= 2000)
            cursor_pos = 0;
    }
}

void
printx(uint32_t val)
{
    char buffer[] = { "0x00000000 " };
    for (int32_t i = 0; i < 8; ++i) {
        int32_t hex = val & 0xF;
        if (hex > 9)
            buffer[9-i] = (0x41 - 10) + hex;
        else
            buffer[9-i] = 0x30 + hex;
        val = val >> 4;
    }
    print(buffer);
}

#endif // __ARCH_H__