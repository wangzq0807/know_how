#include "superblk.h"
#include "hard_disk.h"
#include "asm.h"
#include "string.h"

static uint8_t buffer[512] = {0};

struct SuperBlock {
    uint16_t    inodes;                 // inode number
    uint16_t    nzones;                 // not usable, replaced by zones
    int16_t     imap_blocks;            // blocks need by inode map
    int16_t     zmap_blocks;            // blocks need by zone map
    uint16_t    first_datazone;         // first block number of datazone
    int16_t     log_zone_size;          // log2 of block/zone
    uint32_t    max_file_size;          // max file size
    int16_t     magic;                  // magic
    int16_t     padding;                // not usable
    uint32_t    zones;                  // zone number
};

static struct SuperBlock super_blk;

error_t superblk_load(uint32_t blk)
{
    ata_read(blk << 1, 1, &buffer);
    memcpy(&super_blk, buffer, sizeof(super_blk));
    return 0;
}

uint32_t superblk_get_imap_blocks()
{
    return super_blk.imap_blocks;
}

uint32_t superblk_get_zmap_blocks()
{
    return super_blk.zmap_blocks;
}

uint32_t superblk_get_first_datablk()
{
    return super_blk.first_datazone;
}