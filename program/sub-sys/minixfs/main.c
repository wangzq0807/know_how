#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "bitmap.h"
#include "inode.h"
#include "arch/arch.h"
#include "log.h"
#include "string.h"
#include "buffer.h"
#include "memory.h"

#define ROOT_PARTION    0
void init_filesystem()
{
    partion_load();
    const uint32_t nstart = partion_get_start(ROOT_PARTION);
    const uint32_t superblk_pos = nstart / 2 + 1;
    superblk_load(superblk_pos);

    uint32_t icnt = superblk_get_imap_blocks();
    uint32_t zcnt = superblk_get_zmap_blocks();

    const uint32_t inode_pos = superblk_pos + 1;
    bitmap_inode_load(inode_pos, icnt);
    const uint32_t znode_pos = inode_pos + icnt;
    bitmap_znode_load(znode_pos, zcnt);
}

void start_main()
{
    init_cpu();
    init_memory((void*)(5*1024*1024), (void*)(64*1024*1024));
    init_disk();
    init_block_buffer();
    struct BlockBuffer *buffer = get_block(1, 0);
    printx(buffer->bf_data[510]);
    printx(buffer->bf_data[511]);
    buffer = get_block(1, 1);
    printx(buffer->bf_data[510]);
    printx(buffer->bf_data[511]);
    buffer = get_block(1, 0);
    printx(buffer->bf_data[510]);
    printx(buffer->bf_data[511]);
    // init_filesystem();
    // inode_ls(2);
}