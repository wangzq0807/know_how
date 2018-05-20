#include "defs.h"
#include "log.h"
#include "memory.h"

void
start_main()
{
    init_memory(1024*1024, 5*1024*1024);
    void *page = alloc_page();
    printx((uint32_t)page);
    free_page(page);

    void *page1 = alloc_page();
    void *page2 = alloc_page();
    void *page3;
    printx((uint32_t)page1);
    printx((uint32_t)page2);
    free_page(page1);
    free_page(page2);

    page1 = alloc_page();
    page2 = alloc_page();
    page3 = alloc_page();
    printx((uint32_t)page1);
    printx((uint32_t)page2);
    printx((uint32_t)page3);
    free_page(page2);
    free_page(page1);
    free_page(page3);

    page1 = alloc_page();
    page2 = alloc_page();
    printx((uint32_t)page1);
    printx((uint32_t)page2);

    void * obj1 = malloc(LOG_SIZE(64));
    printx((uint32_t)obj1);
    void * obj2 = malloc(LOG_SIZE(64));
    printx((uint32_t)obj2);
    void * obj3 = malloc(LOG_SIZE(65));
    printx((uint32_t)obj3);
    mfree(obj3, LOG_SIZE(65));
    mfree(obj1, LOG_SIZE(64));
    mfree(obj2, LOG_SIZE(64));

    obj1 = malloc(LOG_SIZE(64));
    printx((uint32_t)obj1);
    obj2 = malloc(LOG_SIZE(64));
    printx((uint32_t)obj2);
    obj3 = malloc(LOG_SIZE(65));
    printx((uint32_t)obj3);

    obj1 = alloc_object(EIndexNode, 64);
    obj2 = alloc_object(EIndexNode, 64);
    obj3 = alloc_object(EIndexNode, 64);
    printx((uint32_t)obj1);
    printx((uint32_t)obj2);
    printx((uint32_t)obj3);

    for (int n = 0; ; ++n) {
        page = alloc_page();
        if (page == NULL) {
            printx(n);
            break;
        }
    }
}