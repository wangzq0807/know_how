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
#include "fs.h"

static void
init_filesystem()
{
    init_partion();
    const uint32_t superblk_pos = get_super_block_pos(ROOT_DEVICE);
    init_super_block(ROOT_DEVICE);
    const struct SuperBlock *super_block = get_super_block(ROOT_DEVICE);

    uint32_t icnt = super_block->sb_imap_blocks;
    uint32_t zcnt = super_block->sb_zmap_blocks;

    const uint32_t inode_pos = superblk_pos + SUPER_BLOCK_SIZE;
    bitmap_inode_load(inode_pos, icnt);
    const uint32_t znode_pos = inode_pos + icnt;
    bitmap_znode_load(znode_pos, zcnt);
}

void
start_main()
{
    init_cpu();
    init_memory(5*1024*1024, 64*1024*1024);
    init_disk();
    init_block_buffer();
    init_filesystem();
    // init_filesystem();
    // inode_ls(2);
}