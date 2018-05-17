#include "superblk.h"
#include "hard_disk.h"
#include "asm.h"
#include "string.h"
#include "partion.h"
#include "log.h"
#include "buffer.h"
#include "fs.h"
#include "partion.h"

static struct SuperBlock super_blk[4];

// NOTE : dev unused
uint32_t
get_super_block_begain(uint16_t dev)
{
    struct PartionEntity *entity = get_partion_entity(ROOT_DEVICE);
    const uint32_t nstart = entity->pe_lba_start / PER_BLOCK_SECTORS;
    const uint32_t superblk_pos = nstart + SUPER_BLOCK_BEGAIN;
    return superblk_pos;
}

// NOTE : dev unused
error_t
init_super_block(uint16_t dev)
{
    error_t ret = 0;
    uint32_t pos = get_super_block_begain(dev);
    struct BlockBuffer *buffer = get_block(dev, pos);
    memcpy(&super_blk[0], buffer->bf_data, sizeof(super_blk));
    if (super_blk[0].sb_magic != MINIX_V2 ) {
        ret = -1;
    }
    release_block(buffer);
    return ret;
}

// NOTE : dev unused
const struct SuperBlock *
get_super_block(uint16_t dev)
{
    return &super_blk[0];
}
