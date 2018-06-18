#ifndef __PAGE_H__
#define __PAGE_H__

#define PAGE_SIZE     (4096)
#define PAGE_CEILING(addr)    (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_FLOOR(addr)      ((addr) & ~(PAGE_SIZE - 1))

#define PAGE_PRESENT        1
#define PAGE_WRITE          2
#define PAGE_USER           1 << 2
#define PAGE_ACCESSED       1 << 5
#define PAGE_DIRTY          1 << 6

struct IrqFrame;

void
on_page_fault(struct IrqFrame *);

#endif // __PAGE_H__