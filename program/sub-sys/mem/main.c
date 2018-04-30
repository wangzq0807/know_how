#include "main.h"
#include "defs.h"

#define PAGE_SIZE     (4096)
#define PG_CEILING(addr)    (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PG_FLOOR(addr)      ((addr) & ~(PAGE_SIZE - 1))

struct MemListHead {
    void            *m_dummyLock;   // fixme: 链表加锁
    struct MemNode  *m_free;
};

struct MemNode {
    struct MemNode  *m_next;
};

struct MemListHead free_memory;

void
pre_init_mem(void *start, void *end)
{
    if ( (start > end)
        || (size_t)(start) & (PAGE_SIZE - 1)
        || (size_t)(end) & (PAGE_SIZE - 1) ) {
        print("wrong memory range\n");
        return;
    }

    free_memory.m_free = NULL;
    /* 建立空闲页表链表 */
    while ((start + PAGE_SIZE) <= end) {
        free_page(start);
        start += PAGE_SIZE;
    }
}

void *
new_page()
{
    /* 返回链表中第一个空闲内存页，同时把头指针指向下一个空闲内存页 */
    struct MemNode *ret = free_memory.m_free;
    if (ret != NULL)
        free_memory.m_free = ret->m_next;
    return ret;
}

void
free_page(void *page)
{
    if (page == NULL)   return;
    if ((size_t)(page) & (PAGE_SIZE - 1)) {
        print("wrong page address\n");
        return;
    }
    /* 将新释放的内存页链接到链表头部 */
    struct MemNode *node = (struct MemNode *)page;
    node->m_next = free_memory.m_free;
    free_memory.m_free = node;
}

void
start_main()
{
    pre_init_mem((void*)(1024*1024), (void*)(5*1024*1024));
    void *page = new_page();
    printx((uint32_t)page);
    free_page(page);

    void *page1 = new_page();
    void *page2 = new_page();
    void *page3;
    printx((uint32_t)page1);
    printx((uint32_t)page2);
    free_page(page1);
    free_page(page2);

    page1 = new_page();
    page2 = new_page();
    page3 = new_page();
    printx((uint32_t)page1);
    printx((uint32_t)page2);
    printx((uint32_t)page3);
    free_page(page2);
    free_page(page1);
    free_page(page3);

    page1 = new_page();
    page2 = new_page();
    printx((uint32_t)page1);
    printx((uint32_t)page2);

    for (int n = 0; ; ++n) {
        page = new_page();
        if (page == NULL) {
            printx(n);
            break;
        }
    }
}