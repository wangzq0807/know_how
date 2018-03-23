#include "inode.h"
#include "hard_disk.h"
#include "asm.h"

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
struct IndexNode root_node;

#define DIR_LEN     30
struct Direction {
    uint16_t    inode;
    char        name[DIR_LEN];
};

error_t inode_load(uint32_t node_blk, uint32_t i)
{
    ata_read(node_blk << 1, 2, buffer);
    memcpy(&root_node, buffer, sizeof(struct IndexNode));
    if (S_ISDIR(root_node.file_mode))
        return 1;
    else if (S_ISREG(root_node.file_mode))
        return 0;

    return 0;
}

error_t inode_root_ls(uint32_t first)
{
    struct Direction dir;
    ata_read((first + root_node.zones[0]) << 1, 2, buffer);
    memcpy(&dir, buffer, sizeof(struct Direction));
    memcpy(&dir, buffer + 32, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*2, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*3, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*4, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*5, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*6, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*7, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*8, sizeof(struct Direction));
    memcpy(&dir, buffer + 32*9, sizeof(struct Direction));
    return 0;
}
