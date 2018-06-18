#include "page.h"
#include "asm.h"
#include "memory.h"
#include "task.h"
#include "string.h"

void
on_page_fault(struct IrqFrame *irq)
{
    struct Task *cur_task = current_task();
    uint32_t linear = get_cr2();
    pde_t *pdt = (pde_t *)cur_task->ts_tss.t_CR3;
    uint32_t npdt = linear >> 22;
    uint32_t npte = (linear >> 12) & 0x400;
    pte_t *pte = (pte_t *)(pdt[npdt] & 0xFFFFF000);

    void *new_page = alloc_page();
    memcpy(new_page, (void *)PAGE_FLOOR(linear), PAGE_SIZE);
    pte[npte] = PAGE_FLOOR((uint32_t)new_page) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
}