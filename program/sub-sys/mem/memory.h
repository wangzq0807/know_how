#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "defs.h"

#define PAGE_SIZE     (4096)
#define PG_CEILING(addr)    (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PG_FLOOR(addr)      ((addr) & ~(PAGE_SIZE - 1))

#define LOG_SIZE(val) ({    \
    int32_t n = (val) - 1;  \
    int32_t ret = 0;        \
    while (n > 0) {         \
        n = n >> 1;         \
        ret += 1;           \
    }                       \
    ret;                    \
})

void init_memory(uint32_t start, uint32_t end);

void * alloc_page(void);

int free_page(void *page);

int get_free_space(void);

void * alloc_object(uint32_t log_size);

int free_n_object(void *obj, uint32_t log_size, uint32_t num);

static inline int free_object(void *obj, uint32_t log_size) {
    return free_n_object(obj, log_size, 1);
}

#endif
