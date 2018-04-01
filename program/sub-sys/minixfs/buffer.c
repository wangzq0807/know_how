#include "buffer.h"
#include "hard_disk.h"

// configurable
#define BUFFER_LIST_LEN 100
#define BUFFER_HASH_LEN 100

#define HASH_MAGIC   (BUFFER_HASH_LEN * 1000 / 618)
#define HASH(val)    ((val)*HASH_MAGIC % BUFFER_HASH_LEN)

static struct Buffer free_buffers[BUFFER_LIST_LEN];
static struct Buffer *hash_map[BUFFER_HASH_LEN];

error_t
setup_block_buffer()
{
    struct Buffer *iter;
    struct Buffer *prev = &free_buffers[0];
    prev->bf_data = (uint8_t*)BUFFER_CACHE;
    prev->bf_refs = 0;
    prev->bf_dev = 0;
    prev->bf_blk = 0;
    prev->bf_hash_prev = NULL;
    prev->bf_hash_next = NULL;
    prev->bf_prev = NULL;
    prev->bf_next = NULL;
    for (int i = 1; i < BUFFER_LIST_LEN; ++i) {
        iter = &free_buffers[i];
        iter->bf_data = (uint8_t*)(BUFFER_CACHE + i*BUFFER_CACHE_SIZE);
        iter->bf_refs = 0;
        iter->bf_dev = 0;
        iter->bf_blk = 0;
        iter->bf_hash_prev = NULL;
        iter->bf_hash_next = NULL;
        iter->bf_prev = prev;
        prev->bf_next = iter;
        prev = iter;
    }
    iter->bf_next = &free_buffers[0];
    free_buffers[0].bf_prev = iter;

    return 0;
}

error_t
remove_hash_entity(struct Buffer *buf)
{
    struct Buffer *hash_prev = buf->bf_hash_prev;
    struct Buffer *hash_next = buf->bf_hash_next;
    if (hash_prev != NULL)
        hash_prev->bf_hash_next = hash_next;
    if (hash_next != NULL)
        hash_next->bf_hash_prev = hash_prev;
    buf->bf_hash_prev = NULL;
    buf->bf_hash_next = NULL;

    return 0;
}

struct Buffer *
get_hash_entity(uint16_t dev, uint32_t blk)
{
    uint32_t hash_val = HASH(blk);
    struct Buffer *buf = hash_map[hash_val];
    while (buf) {
        if (buf->bf_dev == dev &&
            buf->bf_blk == blk)
            return buf;
        buf = buf->bf_hash_next;
    }
    return NULL;
}

error_t
put_hash_entity(struct Buffer *buf)
{
    struct Buffer *org = get_hash_entity(buf->bf_dev, buf->bf_blk);
    if (org != NULL)
        remove_hash_entity(org);

    uint32_t hash_val = HASH(buf->bf_blk);
    struct Buffer *head = hash_map[hash_val];
    buf->bf_hash_next = head;
    buf->bf_hash_prev = NULL;
    if (head != NULL)
        head->bf_hash_prev = buf;
    hash_map[hash_val] = buf;

    return 0;
}

const struct Buffer *
get_block(uint16_t dev, uint32_t blk)
{
    struct Buffer *buf = get_hash_entity(dev, blk);
    if (buf != NULL)
        return buf;
    
}

// error_t put_block(const struct Buffer *buf)
// {
//     lock buffer;
//     block_write(buf->dev, buf->block_num, buf->buffer);
// }

// struct Buffer* alloc_buffer( )
// {
//     struct Buffer *ret = free_bufs;
//     free_bufs = free_bufs->after;

//     ret->prev->after = ret->after;
//     ret->after->prev = ret->prev;

//     ret->prev = NULL;
//     ret->after = NULL;

//     return ret;
// }

// error_t free_buffer(struct Buffer *buf)
// {
//     buf->after = free_bufs;
//     buf->prev = free_bufs->prev;
//     free_bufs->prev->after = buf;
//     free_bufs->prev = buf;
// }
