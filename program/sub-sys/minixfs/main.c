#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "bitmap.h"
#include "inode.h"
#include "arch.h"
#include "log.h"

// void sizecheck()
// {
//     // uint32_t siz = sizeof(uint8_t);
//     char sz[] = "abc";
//     print(sz);
//     // siz = sizeof(uint16_t);
//     // printx(siz);
//     // siz = sizeof(uint32_t);
//     // printx(siz);
//     // siz = sizeof(uint64_t);
//     // printx(siz);
// }

void start_main()
{
    char sz[] = "abc";
    print(sz);
    // sizecheck();
    // init_regs();
    // partion_load();
    // uint32_t nstart = partion_get_start(0);
    // nstart = nstart / 2 + 1;
    // superblk_load(nstart);
    // nstart += 1;
    // uint32_t icnt = superblk_get_imap_blocks();
    // uint32_t zcnt = superblk_get_zmap_blocks();
    // bitmap_inode_load(nstart, icnt);
    // nstart += icnt;
    // bitmap_znode_load(nstart, zcnt);
    // nstart += zcnt;

}