#include "nodes.h"
#include "superblk.h"
#include "buffer.h"
#include "memory.h"
#include "fs.h"
#include "log.h"
#include "string.h"
#include "partion.h"

#define PER_BLOCK_BYTES     (SECTOR_SIZE*PER_BLOCK_SECTORS)
// NOTE : 这里要能整除
#define PER_BLOCK_INODES    (PER_BLOCK_BYTES/sizeof(struct PyIndexNode))
// 内存中inode最大数量
#define MAX_INODE_NUM   1024
// IndexNode的hash表
#define BUFFER_HASH_LEN 100
#define HASH_MAGIC   (BUFFER_HASH_LEN * 1000 / 618)
#define HASH(val)    ((val)*HASH_MAGIC % BUFFER_HASH_LEN)
static struct IndexNode *hash_map[BUFFER_HASH_LEN];

struct BlockBuffer *inode_map[MAX_IMAP_NUM] = {0};
struct BlockBuffer *znode_map[MAX_ZMAP_NUM] = {0};

static inline void
_set_bit(uint8_t *byte, uint32_t num)
{
    const uint32_t val = 1 << (num & 7);
    *byte |= val;
}

static inline uint32_t
_get_bit(uint8_t *byte, uint32_t num)
{
    const uint32_t val = 1 << (num & 7);
    return (*byte) & val;
}

error_t
init_nodes(uint16_t dev)
{
    const uint32_t superblk_begin = get_super_block_begin(dev);
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t icnt = super_block->sb_imap_blocks;
    const uint32_t zcnt = super_block->sb_zmap_blocks;

    const uint32_t inode_begin = superblk_begin + SUPER_BLOCK_SIZE;
    for (int i = 0; i < icnt && i < MAX_IMAP_NUM; ++i) {
        inode_map[i] = get_block(dev, inode_begin + i);
    }
    const uint32_t znode_pos = inode_begin + icnt;
    for (int i = 0; i < zcnt && i < MAX_ZMAP_NUM; ++i) {
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

static inline uint32_t
_get_inode_begin(uint16_t dev)
{
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t superblk_begin = get_super_block_begin(dev);
    const uint32_t superblk_end = superblk_begin + SUPER_BLOCK_SIZE;
    return superblk_end + super_block->sb_imap_blocks + super_block->sb_zmap_blocks;
}

static inline uint32_t
_get_znode_begin(uint16_t dev)
{
    struct PartionEntity *entity = get_partion_entity(ROOT_DEVICE);
    const uint32_t nstart = entity->pe_lba_start / PER_BLOCK_SECTORS;
    const struct SuperBlock *super_block = get_super_block(dev);
    return nstart + super_block->sb_first_datazone;
}

static error_t
_remove_hash_entity(struct IndexNode *inode)
{
    struct IndexNode *hash_prev = inode->in_hash_prev;
    struct IndexNode *hash_next = inode->in_hash_next;
    if (hash_prev != NULL)
        hash_prev->in_hash_next = hash_next;
    if (hash_next != NULL)
        hash_next->in_hash_prev = hash_prev;
    inode->in_hash_prev = NULL;
    inode->in_hash_next = NULL;

    return 0;
}

static struct IndexNode *
_get_hash_entity(uint16_t dev, uint32_t idx)
{
    uint32_t hash_val = HASH(idx);
    struct IndexNode *inode = hash_map[hash_val];
    while (inode != NULL) {
        if (inode->in_dev == dev &&
            inode->in_inum == idx)
            return inode;
        inode = inode->in_hash_next;
    }
    return NULL;
}

static error_t
_put_hash_entity(struct IndexNode *inode)
{
    struct IndexNode *org = _get_hash_entity(inode->in_dev, inode->in_inum);
    if (org != NULL)
        _remove_hash_entity(org);

    uint32_t hash_val = HASH(inode->in_inum);
    struct IndexNode *head = hash_map[hash_val];
    inode->in_hash_next = head;
    inode->in_hash_prev = NULL;
    if (head != NULL)
        head->in_hash_prev = inode;
    hash_map[hash_val] = inode;

    return 0;
}

struct IndexNode *
alloc_inode(uint16_t dev)
{
    // 申请一个新的IndexNode
    struct IndexNode *inode = (struct IndexNode *)alloc_object(EIndexNode, sizeof(struct IndexNode));
    // 为新inode分配一个bit位
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t icnt = super_block->sb_imap_blocks;
    const uint32_t inode_index = _alloc_node(inode_map, icnt);
    // 初始化
    inode->in_dev = dev;
    inode->in_status = INODE_LOCK;
    inode->in_inum = inode_index;
    inode->in_refs = 1;
    _put_hash_entity(inode);
    return inode;
}

struct IndexNode *
get_inode(uint16_t dev, uint16_t inode_index)
{
    struct IndexNode *inode = NULL;
    while (inode == NULL) {
        inode = _get_hash_entity(dev, inode_index);
        if (inode != NULL) {
            if (inode->in_status == INODE_LOCK) {
                // TODO : sleep for inode
                continue;
            }
            inode->in_refs += 1;
            return inode;
        }
        else {
            // 申请一个新的IndexNode
            inode = (struct IndexNode *)alloc_object(EIndexNode, sizeof(struct IndexNode));
            // 读磁盘上的inode
            const uint32_t inode_begin = _get_inode_begin(dev);
            const uint32_t block_num = (inode_index - 1) / PER_BLOCK_INODES + inode_begin;
            const uint32_t offset = (inode_index - 1) % PER_BLOCK_INODES;

            struct BlockBuffer *buffer = get_block(dev, block_num);
            // 将inode的内容拷贝到IndexNode
            uint8_t *ptr = buffer->bf_data + offset * sizeof(struct PyIndexNode);
            memcpy(inode, ptr, sizeof(struct PyIndexNode));
            release_block(buffer);
            // 初始化
            inode->in_dev = dev;
            inode->in_status = 0;
            inode->in_inum = inode_index;
            inode->in_refs = 1;
            _put_hash_entity(inode);
        }
    }
    return (struct IndexNode *)inode;
}

error_t
free_inode(struct IndexNode *inode)
{
    
    return 0;
}

uint32_t
alloc_znode(uint16_t dev)
{
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t zcnt = super_block->sb_zmap_blocks;
    uint32_t znode = _alloc_node(znode_map, zcnt);
    return znode;
}

void *
get_znode(uint16_t dev, uint32_t idx)
{
    struct PartionEntity *entity = get_partion_entity(ROOT_DEVICE);
    const uint32_t nstart = entity->pe_lba_start / PER_BLOCK_SECTORS;
    struct BlockBuffer *buffer = get_block(dev, nstart + idx);
    return buffer->bf_data;
}