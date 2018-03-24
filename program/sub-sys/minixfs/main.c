#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "bitmap.h"
#include "inode.h"

#define SCREEN_ADR  0xB8000    /* 显存地址 */
#define ONE_LINE    160        /* 一行的空间大小 */
#define ONE_PAGE    0x1000     /* 一页的空间大小 */
#define CHAR_PROP   0x0F       /* 字符属性(白色) */
uint32_t cursor_pos = 0;
void print(const char *buffer)
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

void start_main()
{
    partion_load();
    uint32_t nstart = partion_get_start(0);
    nstart = nstart / 2 + 1;
    superblk_load(nstart);
    nstart += 1;
    uint32_t icnt = superblk_get_imap_blocks();
    uint32_t zcnt = superblk_get_zmap_blocks();
    bitmap_inode_load(nstart, icnt);
    nstart += icnt;
    bitmap_znode_load(nstart, zcnt);
    nstart += zcnt;

    inode_ls( 1 );
    inode_ls( 2 );
    inode_ls( 3 );
    inode_ls( 4 );
    inode_ls( 5 );
    inode_ls( 6 );
    inode_ls( 7 );
    inode_ls( 8 );
    inode_ls( 9 );
    inode_ls( 10 );
    inode_ls( 11 );

    char sz[] = "asdfasdfafgds\ngdsgsdfgdsfgsdfdsgsdf\n123445756765456768768578567";
    //for (int i = 0; i < 20; ++i )
    print(sz);
    // ata_read(0, 1, (void *)0x2000);
    // ata_write((void*)0x7c00, 1, 1);
}