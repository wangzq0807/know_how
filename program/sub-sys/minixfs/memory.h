#ifndef __MEMORY_H__
#define __MEMORY_H__
#include "defs.h"

#define PAGE_SIZE     (4096)
#define PG_CEILING(addr)    (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PG_FLOOR(addr)      ((addr) & ~(PAGE_SIZE - 1))

void init_memory(uint32_t start, uint32_t end);

void *new_page();

int free_page(void *page);

int get_free_space();

#endif
