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