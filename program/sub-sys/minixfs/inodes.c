#include "inodes.h"
#include "superblk.h"
#include "buffer.h"
#include "memory.h"
#include "fs.h"
#include "log.h"
#include "string.h"
#include "partion.h"

#define PER_BLOCK_BYTES     (1 << BLOCK_LOG_SIZE)
#define PER_BLOCK_SECTORS   (PER_BLOCK_BYTES/SECTOR_SIZE)
// NOTE : 这里要能整除
#define PER_BLOCK_INODES    (PER_BLOCK_BYTES/sizeof(struct PyIndexNode))
// 内存中inode最大数量
#define MAX_INODE_NUM   1024
// IndexNode的hash表
#define BUFFER_HASH_LEN 100
#define HASH_MAGIC   (BUFFER_HASH_LEN * 1000 / 618)
#define HASH(val)    ((val)*HASH_MAGIC % BUFFER_HASH_LEN)

// 间接索引块数
#define INDIRECT_ZONES      (PER_BLOCK_BYTES / sizeof(uint32_t))
// 双间接索引
#define DINDIRECT_ZONES     (INDIRECT_ZONES * INDIRECT_ZONES)
// 三间接索引
#define TINDIRECT_ZONES     (INDIRECT_ZONES * INDIRECT_ZONES * INDIRECT_ZONES)

struct IndexNodeHead {
    uint32_t            in_lock;
    struct IndexNode    *in_free;
};

static struct IndexNodeHead free_inodes;
static struct IndexNode *ihash_map[BUFFER_HASH_LEN];

struct BlockBuffer *inode_map[MAX_IMAP_NUM] = {0};
// TODO: znode_map数组的大小是不够用的
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
    free_inodes.in_free = NULL;
    free_inodes.in_lock = 0;
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
    struct PartionEntity *entity = get_partion_entity(dev);
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
    struct IndexNode *inode = ihash_map[hash_val];
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
    struct IndexNode *head = ihash_map[hash_val];
    inode->in_hash_next = head;
    inode->in_hash_prev = NULL;
    if (head != NULL)
        head->in_hash_prev = inode;
    ihash_map[hash_val] = inode;

    return 0;
}

struct IndexNode *
alloc_inode(uint16_t dev)
{
    // 申请一个新的IndexNode
    struct IndexNode *inode = NULL;
    if (free_inodes.in_free == NULL)
        inode = (struct IndexNode *)alloc_object(EIndexNode, sizeof(struct IndexNode));
    else {
        inode = free_inodes.in_free;
        free_inodes.in_free = inode->in_next;
    }
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

error_t
free_inode(struct IndexNode *inode)
{

    return 0;
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
            if (free_inodes.in_free == NULL)
                inode = (struct IndexNode *)alloc_object(EIndexNode, sizeof(struct IndexNode));
            else {
                inode = free_inodes.in_free;
                free_inodes.in_free = inode->in_next;
            }
            // 读磁盘上的inode
            const uint32_t inode_begin = _get_inode_begin(dev);
            const uint32_t block_num = (inode_index - 1) / PER_BLOCK_INODES + inode_begin;
            const uint32_t offset = (inode_index - 1) % PER_BLOCK_INODES;

            struct BlockBuffer *buffer = get_block(dev, block_num);
            // 将inode的内容拷贝到IndexNode
            uint8_t *ptr = buffer->bf_data + offset * sizeof(struct PyIndexNode);
            memcpy(&inode->in_inode, ptr, sizeof(struct PyIndexNode));
            release_block(buffer);
            // 初始化
            inode->in_dev = dev;
            inode->in_status = INODE_LOCK;
            inode->in_inum = inode_index;
            inode->in_refs = 1;
            _put_hash_entity(inode);
        }
    }
    return (struct IndexNode *)inode;
}

void
release_inode(struct IndexNode *inode)
{
    inode->in_refs -= 1;
    if (inode->in_refs == 0) {
        // 将inode放入空闲列表
        inode->in_next = free_inodes.in_free;
        free_inodes.in_free = inode;
        _remove_hash_entity(inode);
        // 读磁盘上的inode缓冲区
        const uint32_t inode_begin = _get_inode_begin(inode->in_dev);
        const uint32_t block_num = (inode->in_inum - 1) / PER_BLOCK_INODES + inode_begin;
        const uint32_t offset = (inode->in_inum - 1) % PER_BLOCK_INODES;

        struct BlockBuffer *buffer = get_block(inode->in_dev, block_num);
        // 将IndexNode的内容拷贝到inode缓冲区
        uint8_t *ptr = buffer->bf_data + offset * sizeof(struct PyIndexNode);
        memcpy(ptr, &inode->in_inode, sizeof(struct PyIndexNode));
        release_block(buffer);
    }
    inode->in_status = 0;
}

uint32_t
alloc_zone(uint16_t dev)
{
    const struct SuperBlock *super_block = get_super_block(dev);
    const uint32_t zcnt = super_block->sb_zmap_blocks;
    uint32_t znode = _alloc_node(znode_map, zcnt);
    return znode;
}

uint32_t
get_zone(struct IndexNode *inode, uint32_t bytes_offset, uint32_t *offset_in_blk)
{
    struct PartionEntity *entity = get_partion_entity(inode->in_dev);
    const uint32_t nstart = entity->pe_lba_start / PER_BLOCK_SECTORS;
    // TODO : 这里暂时假设zone和block大小相同
    uint32_t block_num;
    const uint32_t zone_num = bytes_offset >> BLOCK_LOG_SIZE;
    if (offset_in_blk)
        *offset_in_blk = bytes_offset & (PER_BLOCK_BYTES - 1);
    // 直接索引
    if (zone_num < DIRECT_ZONE) {
        block_num = inode->in_inode.in_zones[zone_num];
    }
    // 间接索引
    else if (zone_num < (DIRECT_ZONE + INDIRECT_ZONES)) {
        const uint32_t in_blk = inode->in_inode.in_zones[DIRECT_ZONE];
        const uint32_t in_num = zone_num - DIRECT_ZONE;
        struct BlockBuffer *buf = get_block(inode->in_dev, nstart+in_blk);
        block_num = ((uint32_t *)buf->bf_data)[in_num];
        release_block(buf);
    }
    // 双间接索引
    else if (zone_num < (DIRECT_ZONE + INDIRECT_ZONES + DINDIRECT_ZONES)) {
        uint32_t db_blk = inode->in_inode.in_zones[DIRECT_ZONE + 1];
        uint32_t db_num = zone_num - DIRECT_ZONE - INDIRECT_ZONES;

        uint32_t db_offset = db_num / INDIRECT_ZONES;
        uint32_t db_inoffset = db_num % INDIRECT_ZONES;

        struct BlockBuffer *buf = get_block(inode->in_dev, nstart+db_blk);
        uint32_t inblock_num = ((uint32_t *)buf->bf_data)[db_offset];
        release_block(buf);

        buf = get_block(inode->in_dev, nstart+inblock_num);
        block_num = ((uint32_t *)buf->bf_data)[db_inoffset];
        release_block(buf);
    }
    // 三间接索引
    else if (zone_num < (DIRECT_ZONE + INDIRECT_ZONES + DINDIRECT_ZONES + TINDIRECT_ZONES)) {
        uint32_t tr_blk = inode->in_inode.in_zones[DIRECT_ZONE + 2];
        uint32_t tr_num = zone_num - DIRECT_ZONE - INDIRECT_ZONES - DINDIRECT_ZONES;

        uint32_t tr_offset = tr_num / DINDIRECT_ZONES;
        uint32_t tr_inoffset = (tr_num % DINDIRECT_ZONES)/INDIRECT_ZONES;
        uint32_t tr_dboffset = tr_num % INDIRECT_ZONES;

        struct BlockBuffer *buf = get_block(inode->in_dev, nstart+tr_blk);
        uint32_t inblock_num = ((uint32_t *)buf->bf_data)[tr_offset];
        release_block(buf);

        buf = get_block(inode->in_dev, nstart+inblock_num);
        uint32_t dbblock_num = ((uint32_t *)buf->bf_data)[tr_inoffset];
        release_block(buf);

        buf = get_block(inode->in_dev, nstart+dbblock_num);
        block_num = ((uint32_t *)buf->bf_data)[tr_dboffset];
        release_block(buf);
    }

    return nstart + block_num;
}

