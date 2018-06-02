#include "defs.h"
#include "hard_disk.h"
#include "partion.h"
#include "superblk.h"
#include "inodes.h"
#include "arch/arch.h"
#include "log.h"
#include "string.h"
#include "buffer.h"
#include "memory.h"
#include "fs.h"
#include "sys/stat.h"
#include "zones.h"

static void
init_filesystem(uint16_t dev)
{
    init_partion(dev);
    init_super_block(dev);
    dump_super_block(dev);
    init_inodes(dev);

    struct IndexNode *inode = get_inode(dev, 1);
    uint32_t blk = 0;
    uint32_t offset = 0;
    blk = get_zone(inode, 0, &offset);

    struct BlockBuffer *buf = get_block(inode->in_dev, blk);
    uint8_t *data = buf->bf_data;

    struct Direction dir;
    uint32_t file_seek = 0;
    while (file_seek < inode->in_inode.in_file_size) {
        memcpy(&dir, data+file_seek, sizeof(struct Direction));
        print(dir.name);
        print(" ");

        struct IndexNode *one_inode = get_inode(dev, dir.inode);
        if (S_ISREG(one_inode->in_inode.in_file_mode)) {
            uint32_t bytes = one_inode->in_inode.in_file_size - 3;
            blk = get_zone(one_inode, bytes, &offset);
            printx(blk);
            struct BlockBuffer *one_buf = get_block(one_inode->in_dev, blk);
            char *content = (char *)one_buf->bf_data + offset;
            print(content);
            release_block(one_buf);
        }
        print("\n");
        file_seek += sizeof(struct Direction);
    }
    release_block(buf);
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