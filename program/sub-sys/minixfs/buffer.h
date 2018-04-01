#ifndef __BUFFER_H__
#define __BUFFER_H__
#include "defs.h"

#define BUFFER_CACHE        0x100000    // cache begin: 1M
#define BUFFER_CACHE_END    0x500000    // cache end: 5M
#define BUFFER_CACHE_SIZE   1024        // bytes cnt of per cache

struct Buffer {
    uint8_t *bf_data;
    uint16_t bf_refs;
    uint16_t bf_dev;            // dev num
    uint32_t bf_blk;            // block num
    uint32_t bf_status;
    struct Buffer *bf_hash_prev;
    struct Buffer *bf_hash_next;
    struct Buffer *bf_prev;
    struct Buffer *bf_next;
};

error_t setup_block_buffer();

struct Buffer *alloc_buffer();

error_t free_buffer(struct Buffer *buf);

const struct Buffer *get_block(uint16_t dev, uint32_t blk);

error_t put_block(const struct Buffer *buf);

#endif // __BUFFER_H__
