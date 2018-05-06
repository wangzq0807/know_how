#include "bitmap.h"
#include "hard_disk.h"

// NOTE: 最大支持32M硬盘
#define MAP_SIZE 4096

uint32_t inode_map_pos = 0;
uint32_t inode_map_size = 0;
uint32_t znode_map_pos = 0;
uint32_t znode_map_size = 0;

uint8_t inode_map[MAP_SIZE] = {0xFF};
uint8_t znode_map[MAP_SIZE] = {0xFF};

error_t bitmap_inode_load(uint32_t inode_blk, uint32_t cnt)
{
    inode_map_pos = inode_blk;
    inode_map_size = cnt;
    // ata_read(inode_blk << 1, cnt << 1, inode_map);
    return 0;
}

uint32_t bitmap_inode_alloc()
{
    for (uint32_t byte = 0; byte < MAP_SIZE; ++byte) {
        for (uint32_t bit = 0; bit < 8; ++bit) {
            const uint8_t test_bit = 1 << bit;
            if ((inode_map[byte] & test_bit) == 0 ) {
                inode_map[byte] |= test_bit;

                // const uint32_t sector = byte >> 9;
                // const uint32_t sector_byte = sector << 9;
                // ata_write(&inode_map[sector_byte], (inode_map_pos << 1) + sector, 1);

                return (byte << 3) + bit;
            }
        }
    }
    return 0;
}

error_t bitmap_inode_delete(uint32_t index)
{
    if (index > ((MAP_SIZE << 3) - 1))
        return -1;

    const uint32_t byte = index >> 3;
    const uint8_t bit = index % 8;
    const uint8_t test_bit = 1 << bit;
    inode_map[byte] &= ~test_bit;

    // const uint32_t sector = byte >> 9;
    // const uint32_t sector_byte = sector << 9;
    // ata_write(&inode_map[sector_byte], (inode_map_pos << 1) + sector, 1);

    return 0;
}

error_t bitmap_znode_load(uint32_t znode_blk, uint32_t cnt)
{
    znode_map_pos = znode_blk;
    znode_map_size = cnt;
    // ata_read(znode_blk << 1, cnt << 1, znode_map);
    return 0;
}

uint32_t bitmap_znode_alloc()
{
    for (uint32_t byte = 0; byte < MAP_SIZE; ++byte) {
        for (uint32_t bit = 0; bit < 8; ++bit) {
            const uint8_t test_bit = 1 << bit;
            if ((znode_map[byte] & test_bit) == 0 ) {
                znode_map[byte] |= test_bit;

                // const uint32_t sector = byte >> 9;
                // const uint32_t sector_byte = sector << 9;
                // ata_write(&inode_map[sector_byte], (znode_map_pos << 1) + sector, 1);

                return (byte << 3) + bit;
            }
        }
    }
    return 0;
}