#include "memory.h"
#include "log.h"

struct PageListHead {
    uint32_t         pl_dummyLock;   // fixme: 链表加锁
    struct PageNode  *pl_free;
};

struct PageNode {
    struct PageNode  *pn_next;
};

struct PageListHead free_memory;

void
init_memory(uint32_t start, uint32_t end)
{
    if ( (start > end)
        || (size_t)(start) & (PAGE_SIZE - 1)
        || (size_t)(end) & (PAGE_SIZE - 1) ) {
        print("wrong memory range\n");
        return;
    }

    free_memory.pl_free = NULL;
    /* 建立空闲页表链表 */
    while ((start + PAGE_SIZE) <= end) {
        free_page((void*)start);
        start += PAGE_SIZE;
    }
}

void *
alloc_page()
{
    /* 返回链表中第一个空闲内存页，同时把头指针指向下一个空闲内存页 */
    struct PageNode *ret = free_memory.pl_free;
    if (ret != NULL)
        free_memory.pl_free = ret->pn_next;
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
    struct PageNode *node = (struct PageNode *)page;
    node->pn_next = free_memory.pl_free;
    free_memory.pl_free = node;
    return 0;
}

int
get_free_space()
{
    struct PageNode *node = free_memory.pl_free;
    int cnt = 0;
    while (node != NULL) {
        cnt++;
        node = node->pn_next;
    }
    return cnt;
}


/*************************
 * 对象管理：
 * 按2的次幂为单位来分配内存
 *************************/
#define MIN_LOG_SIZE    5       // 对象最小为32字节
#define MAX_LOG_SIZE    10      // 对象最大为1024字节
#define OBJ_LIST_LEN    (MAX_LOG_SIZE - MIN_LOG_SIZE + 1)

struct MemObject {
    struct MemObject    *mo_next;
};

struct MemObjectHead {
    uint32_t           mo_dummyLock;
    struct MemObject  *mo_free;
};
// 为每个2的次幂内存块建一个链表，记录空闲的内存块
struct MemObjectHead   object_list[OBJ_LIST_LEN];

void *
alloc_object(uint32_t log_size)
{
    if (log_size < MIN_LOG_SIZE || log_size > MAX_LOG_SIZE)
        return NULL;
    struct MemObjectHead *free_list = &object_list[log_size - MIN_LOG_SIZE];
    if (free_list->mo_free == NULL) {
        void *page = alloc_page();
        if (page == NULL)
            return NULL;
        free_n_object(page, log_size, PAGE_SIZE >> log_size);
    }
    struct MemObject *ret = free_list->mo_free;
    free_list->mo_free = ret->mo_next;
    return ret; 
}

int
free_n_object(void *obj, uint32_t log_size, uint32_t num)
{
    if (log_size < MIN_LOG_SIZE || log_size > MAX_LOG_SIZE)
        return -1;
    const uint32_t objsize = 1 << log_size;
    uint8_t *byte_ptr = (uint8_t*)obj;
    struct MemObjectHead *free_list = &object_list[log_size - MIN_LOG_SIZE];
    for (int n = 0; n < num; ++n) {
        ((struct MemObject *)byte_ptr)->mo_next = free_list->mo_free;
        free_list->mo_free = ((struct MemObject *)byte_ptr);
        byte_ptr += objsize;
    }
    return 0;
}
