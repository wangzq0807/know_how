#include "memory.h"
#include "log.h"

struct MemListHead {
    void            *m_dummyLock;   // fixme: 链表加锁
    struct MemNode  *m_free;
};

struct MemNode {
    struct MemNode  *m_next;
};

struct MemListHead free_memory;

void
init_memory(uint32_t start, uint32_t end)
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
        free_page((void*)start);
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

int
free_page(void *page)
{
    if (page == NULL)   return -1;
    if ((size_t)(page) & (PAGE_SIZE - 1)) {
        print("wrong page address\n");
        return -1;
    }
    /* 将新释放的内存页链接到链表头部 */
    struct MemNode *node = (struct MemNode *)page;
    node->m_next = free_memory.m_free;
    free_memory.m_free = node;
    return 0;
}
