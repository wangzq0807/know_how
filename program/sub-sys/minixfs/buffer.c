#include "buffer.h"
#include "hard_disk.h"
#include "mem.h"

// configurable
#define BUFFER_LIST_LEN 100
#define BUFFER_HASH_LEN 100

#define HASH_MAGIC   (BUFFER_HASH_LEN * 1000 / 618)
#define HASH(val)    ((val)*HASH_MAGIC % BUFFER_HASH_LEN)

static struct BlockBuffer *free_buffers;
static struct BlockBuffer *hash_map[BUFFER_HASH_LEN];

static struct BlockBuffer *alloc_buffer( );

error_t
init_block_buffer()
{
    struct BlockBuffer *iter;
    struct BlockBuffer *prev = &free_buffers[0];
    prev->bf_data = (uint8_t*)BLK_BUFFER;

    for (int i = 1; i < BUFFER_LIST_LEN; ++i) {
        iter = &free_buffers[i];
        iter->bf_data = (uint8_t*)(BLK_BUFFER + i*BLK_BUFFER_SIZE);

        iter->bf_prev = prev;
        prev->bf_next = iter;
        prev = iter;
    }
    // 连接链表的头和尾
    iter->bf_next = &free_buffers[0];
    free_buffers[0].bf_prev = iter;

    return 0;
}

error_t
remove_hash_entity(struct BlockBuffer *buf)
{
    struct BlockBuffer *hash_prev = buf->bf_hash_prev;
    struct BlockBuffer *hash_next = buf->bf_hash_next;
    if (hash_prev != NULL)
        hash_prev->bf_hash_next = hash_next;
    if (hash_next != NULL)
        hash_next->bf_hash_prev = hash_prev;
    buf->bf_hash_prev = NULL;
    buf->bf_hash_next = NULL;

    return 0;
}

struct BlockBuffer *
get_hash_entity(uint16_t dev, uint32_t blk)
{
    uint32_t hash_val = HASH(blk);
    struct BlockBuffer *buf = hash_map[hash_val];
    while (buf != NULL) {
        if (buf->bf_dev == dev &&
            buf->bf_blk == blk)
            return buf;
        buf = buf->bf_hash_next;
    }
    return NULL;
}

error_t
put_hash_entity(struct BlockBuffer *buf)
{
    struct BlockBuffer *org = get_hash_entity(buf->bf_dev, buf->bf_blk);
    if (org != NULL)
        remove_hash_entity(org);

    uint32_t hash_val = HASH(buf->bf_blk);
    struct BlockBuffer *head = hash_map[hash_val];
    buf->bf_hash_next = head;
    buf->bf_hash_prev = NULL;
    if (head != NULL)
        head->bf_hash_prev = buf;
    hash_map[hash_val] = buf;

    return 0;
}

const struct BlockBuffer *
get_block(uint16_t dev, uint32_t blk)
{
    // 参考《unix操作系统设计》的五种场景。
    // 1. 在Hash表中找到了指定block，并且这个block是空闲的
    struct BlockBuffer *buf = get_hash_entity(dev, blk);
    if (buf != NULL) {
        if (buf->bf_status == BUF_BUSY) {
            // 5. 缓冲区状态为"busy"
        }
        else if (buf->bf_status == BUF_FREE) {
            buf->bf_status = BUF_BUSY;
            // TODO: rw block
        }

        return buf;
    }

    // 2. Hash表中没有找到指定block，新申请一块空的缓冲区
    struct BlockBuffer * new_buffer = alloc_buffer();
    // 3. 新申请的缓冲区的状态是"delay write"，因此需要先写入，然后申请另一块
    // TODO : 写磁盘，写完成后重新放入free list
    new_buffer = alloc_buffer();
    // 4. free list已经为空


    return new_buffer;
}

// error_t put_block(const struct Buffer *buf)
// {
//     lock buffer;
//     block_write(buf->dev, buf->block_num, buf->buffer);
// }

static struct BlockBuffer *
alloc_buffer( )
{
    if (free_buffers == NULL
        || free_buffers->bf_next == free_buffers
        || free_buffers->bf_prev == free_buffers)
        return NULL;

    struct BlockBuffer *ret = free_buffers;
    free_buffers = free_buffers->bf_next;

    if (ret->bf_prev != NULL)
        ret->bf_prev->bf_next = ret->bf_next;
    if (ret->bf_next != NULL)
        ret->bf_next->bf_prev = ret->bf_prev;

    ret->bf_prev = NULL;
    ret->bf_next = NULL;
    ret->bf_refs = 1;

    return ret;
}

// error_t free_block(struct Buffer *buf)
// {
//     buf->after = free_bufs;
//     buf->prev = free_bufs->prev;
//     free_bufs->prev->after = buf;
//     free_bufs->prev = buf;
// }
