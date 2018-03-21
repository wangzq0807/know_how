#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "bitmap.h"

#define SCREEN_ADR  0xB8000    /* 显存地址 */
#define ONE_LINE    160        /* 一行的空间大小 */
#define ONE_PAGE    0x1000     /* 一页的空间大小 */
#define CHAR_PROP   0x0F       /* 字符属性(白色) */
uint32_t cursor_pos = 0;
void print(const char *buffer, uint32_t cnt)
{
    for (uint32_t i = 0; i < cnt; ++i) {
        char* next_addr = (char*)(SCREEN_ADR + (cursor_pos << 1));
        if (buffer[i] == '\n') {
            cursor_pos = (cursor_pos + 80) / 80 * 80;
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

#define SUPER_ADDR 8+2

void start_main()
{
    partion_load();
    uint32_t nstart = partion_get_start(0);
    nstart += 2;
    superblk_load(nstart);
    uint32_t icnt = superblk_get_imap_blocks(1);
    uint32_t zcnt = superblk_get_imap_blocks(1);
    bitmap_inode_load(nstart+2, icnt);
    bitmap_znode_load(nstart+2+(icnt<<1), zcnt);

    char sz[] = "asdfasdfafgds\ngdsgsdfgdsfgsdfdsgsdf\n123445756765456768768578567";
    //for (int i = 0; i < 20; ++i )
    print(sz, sizeof(sz));
    // ata_read(0, 1, (void *)0x2000);
    // ata_write((void*)0x7c00, 1, 1);
}