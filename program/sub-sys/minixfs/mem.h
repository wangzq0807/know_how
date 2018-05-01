#ifndef __MEM_H__
#define __MEM_H__

#define PAGE_SIZE     (4096)
#define PG_CEILING(addr)    (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PG_FLOOR(addr)      ((addr) & ~(PAGE_SIZE - 1))

void pre_init_mem(void *start, void *end);

void *new_page();

void free_page(void *page);

#endif
