#include "syscall.h"
#include "irq.h"
#include "asm.h"
#include "task.h"
#include "elf.h"
#include "log.h"
#include "page.h"
#include "memory.h"

static inline void
push_str(const char *str) {
    const int *num = (const int *)str;
    int len = 5;
    int cnt = len / sizeof(int) + 1;
    while (cnt--) {
        __asm__("pushl %0"::"r"(*num++):"memory");
    }
}

#define ELF_FILE        (0xD000)

int
knl_exec(struct IrqFrame *irqframe)
{
    struct ElfHeader *elfheader = (struct ElfHeader *)(ELF_FILE);
    printx(elfheader->eh_entry);
    // struct ProgHeader *progheader = (struct ProgHeader *)(ELF_FILE + elfheader->eh_prog_header);

    struct Task *cur_task = current_task();
    pde_t *pdt = (pde_t *)cur_task->ts_tss.t_CR3;
    uint32_t npdt = elfheader->eh_entry >> 22;
    uint32_t npte = (elfheader->eh_entry >> 12) & 0x3FF;
    printx(npdt);
    printx(npte);
    if ( (pdt[npdt] & PAGE_PRESENT) == 0) {
        uint32_t new_page = (uint32_t)alloc_page();
        pdt[npdt] = PAGE_FLOOR(new_page) | PAGE_WRITE | PAGE_USER | PAGE_PRESENT;
    }
    pte_t *pte = (pte_t *)(pdt[npdt] & 0xFFFFF000);
    pte[npte++] = PAGE_FLOOR(ELF_FILE) | PAGE_WRITE | PAGE_USER | PAGE_PRESENT;
    pte[npte++] = PAGE_FLOOR(ELF_FILE+PAGE_SIZE) | PAGE_WRITE | PAGE_USER | PAGE_PRESENT;
    pte[npte++] = PAGE_FLOOR(ELF_FILE+2*PAGE_SIZE) | PAGE_WRITE | PAGE_USER | PAGE_PRESENT;
    pte[npte++] = PAGE_FLOOR(ELF_FILE+3*PAGE_SIZE) | PAGE_WRITE | PAGE_USER | PAGE_PRESENT;
    irqframe->if_EIP = elfheader->eh_entry;
    load_cr3(pdt);

    return 0;
}