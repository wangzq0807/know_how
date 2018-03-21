#include "bitmap.h"
#include "hard_disk.h"

uint8_t inode_map[4096] = {1};
uint8_t znode_map[4096] = {1};

error_t bitmap_inode_load(uint32_t lba_start, uint32_t cnt)
{
    ata_read(lba_start, cnt << 1, inode_map);
    return 0;
}

error_t bitmap_znode_load(uint32_t lba_start, uint32_t cnt)
{
    ata_read(lba_start, cnt << 1, znode_map);
    return 0;
}