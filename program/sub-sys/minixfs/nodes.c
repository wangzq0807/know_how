#include "nodes.h"
#include "superblk.h"
#include "buffer.h"
#include "memory.h"
#include "fs.h"

#define PER_BLOCK_BYTES     (SECTOR_SIZE*PER_BLOCK_SECTORS)

struct BlockBuffer *inode_map[MAX_IMAP_NUM] = {0};
struct BlockBuffer *znode_map[MAX_IMAP_NUM] = {0};

static inline void
_set_bit(uint8_t *byte, uint32_t num)
{
    const uint32_t val = 1 << (num & 8);
    *byte |= val;
}

static inline uint32_t
_get_bit(uint8_t *byte, uint32_t num)
{
    const uint32_t val = 1 << (num & 8);
    return (*byte) &= val;
}

error_t
init_nodes(uint16_t dev)
{
    const uint32_t superblk_begain = get_super_block_begain(dev);
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t icnt = super_block->sb_imap_blocks;
    const uint32_t zcnt = super_block->sb_zmap_blocks;

    const uint32_t inode_pos = superblk_begain + SUPER_BLOCK_SIZE;
    for (int i = 0; i < icnt; ++i) {
        inode_map[i] = get_block(dev, inode_pos + i);
    }
    const uint32_t znode_pos = inode_pos + icnt;
    for (int i = 0; i < zcnt; ++i) {
        znode_map[i] = get_block(dev, znode_pos + i);
    }
    return 0;
}

static uint32_t
_alloc_node(struct BlockBuffer **node_map, uint32_t cnt)
{
    for (uint32_t blk = 0; blk < cnt; ++blk) {
        struct BlockBuffer *buffer = node_map[blk];
        for (uint32_t byte = 0; byte < PER_BLOCK_BYTES; ++byte) {
            for (uint32_t bit = 0; bit < 8; ++bit) {
                if (_get_bit(&buffer->bf_data[byte], bit) == 0) {
                    _set_bit(&buffer->bf_data[byte], bit);
                    return ((blk * PER_BLOCK_BYTES) << 3) + (byte << 3) + bit;
                }
            }
        }
    }
    return 0;
}

uint32_t
alloc_inode(uint16_t dev)
{
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t icnt = super_block->sb_imap_blocks;

    return _alloc_node(inode_map, icnt);
}

uint32_t
alloc_znode(uint16_t dev)
{
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t zcnt = super_block->sb_zmap_blocks;
    return _alloc_node(znode_map, zcnt);
}