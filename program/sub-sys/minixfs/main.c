#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "nodes.h"
#include "inode.h"
#include "arch/arch.h"
#include "log.h"
#include "string.h"
#include "buffer.h"
#include "memory.h"
#include "fs.h"

static void
init_filesystem(uint16_t dev)
{
    init_partion(dev);
    init_super_block(dev);

}

void
start_main()
{
    init_cpu();
    init_memory(5*1024*1024, 64*1024*1024);
    init_disk();
    init_block_buffer();
    init_filesystem(ROOT_DEVICE);
    // init_filesystem();
    // inode_ls(2);
}