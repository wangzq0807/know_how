#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "nodes.h"
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
    init_nodes(dev);
    dump_super_block(dev);
    struct IndexNode *inode = get_inode(dev, 1);
    void *data = get_znode(dev, inode->in_inode.in_zones[0]);
    struct Direction dir;
    uint32_t file_seek = 0;
    while (file_seek < inode->in_inode.in_file_size) {
        memcpy(&dir, data+file_seek, sizeof(struct Direction));
        print(dir.name);
        print("\n");
        file_seek += sizeof(struct Direction);
    }
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