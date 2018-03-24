#include "inode.h"
#include "hard_disk.h"
#include "asm.h"
#include "partion.h"
#include "superblk.h"

#define DIRECT_ZONE 7
#define NUMBER_ZONE 10

struct IndexNode {
    uint16_t    file_mode;      // file type, auth,
    int16_t     num_links;      // the number of linking to this file
    int16_t     owner_id;       // owner's id
    int8_t      group_id;       // goup's id
    uint32_t    file_size;      // file size in types
    time_t      atime;          // access time
    time_t      mtime;          // modify time
    time_t      ctime;          // create time
    uint32_t    zones[NUMBER_ZONE];      // zone
};
static uint8_t buffer[1024] = {0};

#define DIR_LEN     30
struct Direction {
    uint16_t    inode;
    char        name[DIR_LEN];
};

uint32_t inode_get_blknum(uint32_t inode)
{
    uint32_t nstart = partion_get_start(0) / 2;
    nstart += 2;    // begin + boot + super;
    nstart += superblk_get_imap_blocks();
    nstart += superblk_get_zmap_blocks();
    return nstart + ((inode - 1) * sizeof(struct IndexNode)) / 1024;
}

struct IndexNode inode_load(uint32_t inode)
{
    uint32_t blknum = inode_get_blknum(inode);
    ata_read(blknum << 1, 2, buffer);

    struct IndexNode root_node;
    memcpy(&root_node, buffer + (inode -1)*sizeof(struct IndexNode), sizeof(struct IndexNode));
    if (S_ISDIR(root_node.file_mode))
        return root_node;
    else if (S_ISREG(root_node.file_mode))
        return root_node;

    return root_node;
}

error_t inode_data_load(uint32_t inode, uint8_t *bf)
{
    struct IndexNode root_node = inode_load(inode);
    uint32_t blknum = root_node.zones[0] + partion_get_start(0) / 2;
    ata_read(blknum << 1, 2, bf);
    return 0;
}

error_t inode_ls(uint32_t inode)
{
    if (inode == 0) return 0;

    struct Direction dir;
    inode_data_load(inode, buffer);

    char rt[] = "    ";
    memcpy(&dir, buffer, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*2, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*3, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*4, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*5, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*6, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*7, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*8, sizeof(struct Direction));
    print(dir.name); print(rt);
    memcpy(&dir, buffer + 32*9, sizeof(struct Direction));
    print(dir.name); print(rt);

    return 0;
}
