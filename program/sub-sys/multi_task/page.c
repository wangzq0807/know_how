#include "page.h"
#include "asm.h"
#include "memory.h"
#include "task.h"
#include "string.h"
#include "log.h"

void
on_page_fault(struct IrqFrame *irq)
{
    struct Task *cur_task = current_task();
    if (cur_task == NULL) {
        return;
    }

    pde_t *pdt = (pde_t *)cur_task->ts_tss.t_CR3;
    uint32_t linear = PAGE_FLOOR(get_cr2());
    uint32_t npdt = linear >> 22;
    uint32_t npte = (linear >> 12) & 0x3FF;
    pte_t *pte = (pte_t *)(pdt[npdt] & 0xFFFFF000);

    void *new_page = alloc_page();
    for (int i = 0; i < 1024; ++i) {
        ((int *)new_page)[i] = ((int *)linear)[i];
    }

    pte[npte] = PAGE_FLOOR((uint32_t)new_page) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;

    load_cr3(pdt);
}