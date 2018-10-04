#include "page.h"
#include "asm.h"
#include "memory.h"
#include "task.h"
#include "string.h"
#include "log.h"

static void
page_copy(void *dist, const void *src, size_t num)
{
    int *dist_page = (int *)PAGE_FLOOR((int)dist);
    const int *src_page = (const int *)PAGE_FLOOR((int)src);
    size_t len = num * PAGE_SIZE / sizeof(int);
    while (--len) {
        *dist_page++ = *src_page++;
    }
}

void
on_page_fault(struct IrqFrame *irq)
{
    struct Task *cur_task = current_task();
    if (cur_task == NULL) {
        return;
    }

    pde_t *pdt = (pde_t *)cur_task->ts_tss.t_CR3;
    uint32_t linear = PAGE_FLOOR(get_cr2());
    printx(linear);
    uint32_t npdt = linear >> 22;
    uint32_t npte = (linear >> 12) & 0x3FF;
    pte_t *pte = (pte_t *)(pdt[npdt] & 0xFFFFF000);

    void *new_page = alloc_page();
    page_copy(new_page, (void *)linear, 1);

    pte[npte] = PAGE_FLOOR((uint32_t)new_page) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;

    load_cr3(pdt);
}