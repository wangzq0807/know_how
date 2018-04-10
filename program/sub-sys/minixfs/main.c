#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "bitmap.h"
#include "inode.h"
#include "arch.h"
#include "log.h"

void start_main()
{
    init_regs();
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