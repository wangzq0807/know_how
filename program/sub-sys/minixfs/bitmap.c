#include "bitmap.h"
#include "hard_disk.h"

uint8_t inode_map[4096] = {1};
uint8_t znode_map[4096] = {1};

error_t bitmap_inode_load(uint32_t inode_blk, uint32_t cnt)
{
    ata_read(inode_blk << 1, cnt << 1, inode_map);
    return 0;
}

error_t bitmap_znode_load(uint32_t znode_blk, uint32_t cnt)
{
    ata_read(znode_blk << 1, cnt << 1, znode_map);
    return 0;
}