#include "task.h"
#include "log.h"
#include "asm.h"
#include "lock.h"
#include "memory.h"
// #include "page.h"

/* 任务一 */
struct Task task1;
/* 任务二 */
struct Task task2;

static void task_1();
static void task_2();

// 竞争资源
struct Mutex one_mutex;
uint32_t conf_res1 = 1;
uint32_t conf_res2 = 1;

/* 当前任务 */
uint32_t current = 1;

static void
_init_8259A()
{
    /* init_ICW1: 00010001b,多片级联,使用ICW4 */
    outb(0x11, 0x20);
    outb(0x11, 0xA0);
    /* init_ICW2: 中断号 0x20 - 0x27, 0x28 - 0x2f */
    outb(0x20, 0x21);
    outb(0x28, 0xA1);
    /* init_ICW3: 100b, IR2 接从片 */
    outb(0x4, 0x21);        /* 100b, IR2 接从片 */
    outb(0x2, 0xA1);        /* 接主片IR2 */
    /* init_ICW4: 普通全嵌套, 非缓冲, 非自动结束 */
    outb(0x1, 0x21);        /* 普通全嵌套, 非缓冲, 非自动结束 */
    outb(0x1, 0xA1);
}

static void 
_init_timer()
{
    /* 写入控制字, 使用方式3, 计数器0 */
    outb(0x36, 0x43);
    /* 写入计数值，设置每10ms中断一次 */
    uint8_t low = BYTE1(11930);
    outb(low, 0x40);    /* 先写低字节 */
    uint8_t high = BYTE2(11930);
    outb(high, 0x40);   /* 后写高字节 */
}

void
switch_task()
{
    // NOTE：task1经过跳转到task2，task2再跳转回task1时，会从ljmp的下一条指令开始执行。
    // NOTE：下面这种切换方式是抢占式的，单核情况下，当两个线程需要同步时，加锁必须是原子操作
    // NOTE：对于非抢占式的内核，加锁无需原子操作
    struct Task *cur = current_task();
    if (cur == &task1) {
        current = 2;
        switch_tss(&task2.ts_tss, task2.ts_ldt);
    }
    else if (cur == &task2) {
        current = 1;
        switch_tss(&task1.ts_tss, task1.ts_ldt);
    }
}

static void
task_1()
{
    __asm__ volatile (
        "mov $0x17, %%ax \n"
        "mov %%ax, %%ds \n"
        : : : "%eax"
    );

    while( 1 ) {
        if (acquire_mutex(&one_mutex) == 0) {
            // 下面是受保护的代码
            conf_res1 = 1111;
            conf_res2 = 2222;
            __asm__ volatile("nop":::"memory");
            if (conf_res1 != 1111 || conf_res2 != 2222)
                print("A");
            else
                print("C");
            release_mutex(&one_mutex);
        }
        else {
            __asm__ volatile("int $0x80");
        }
        // 延时
        // NOTE : 如果从释放锁到重新申请锁的时间过短，
        // 那么其他线程获得锁的几率就会非常小。
        int cnt = 100000;
        while(cnt--)
            pause();
    }
}

static void
task_2()
{
    while( 1 ) {
        if (acquire_mutex(&one_mutex) == 0) {
            // 下面是受保护的代码
            conf_res2 = 4444;
            conf_res1 = 3333;
            __asm__ volatile("nop":::"memory");
            if (conf_res1 != 3333 || conf_res2 != 4444)
                print("B");
            else
                print("D");
            release_mutex(&one_mutex);
        }
        else {
            __asm__ volatile("int $0x80");
        }
        // 延时
        // NOTE : 如果从释放锁到重新申请锁的时间过短，
        // 那么其他线程获得锁的几率就会非常小。
        int cnt = 100000;
        while(cnt--)
            pause();
    }
}

#define PAGE_ADDR(addr)     (addr & 0xfffff000)
#define PAGE_PRESENT        1
#define PAGE_WRITE          2
#define PAGE_USER           4

static void
setup_first_task()
{
    const uint8_t USR_DPL = 0xF;
    setup_code_desc(&task1.ts_ldt[1], 0, 0xFFFFF, USR_DPL);
    setup_data_desc(&task1.ts_ldt[2], 0, 0xFFFFF, USR_DPL);
    uint8_t *ks_page = alloc_page();
    uint8_t *us_page = alloc_page();
    ((uint32_t *)ks_page)[0] = (uint32_t)&task1;

    task1.ts_user_stack = us_page;
    task1.ts_tss.t_SS_0 = KNL_DS;
    task1.ts_tss.t_ESP_0 = (uint32_t)&ks_page[PAGE_SIZE-1];

    uint32_t *pdt = (uint32_t*)alloc_page();
    uint32_t *pte = (uint32_t*)alloc_page();
    pdt[0] = PAGE_ADDR((uint32_t)pte) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    uint32_t addr = 0;
    for (int i = 0; i < 1024; ++i) {
        pte[i] = PAGE_ADDR(addr) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        addr += PAGE_SIZE;
    }
    asm volatile (
        "movl %%eax, %%cr3 \n"
        ::"a"(pdt)
    );
    // Note: 任务切换时,CR3不会被自动保存
    task1.ts_tss.t_CR3 = (uint32_t)pdt;
    task1.ts_tss.t_LDT = KNL_LDT;
}

static void
setup_second_task()
{
    const uint8_t USR_DPL = 0xF;
    setup_code_desc(&task2.ts_ldt[1], 0, 0xFFFFF, USR_DPL);
    setup_data_desc(&task2.ts_ldt[2], 0, 0xFFFFF, USR_DPL);
    uint8_t *ks_page = alloc_page();
    uint8_t *us_page = alloc_page();
    ((uint32_t *)ks_page)[0] = (uint32_t)&task2;

    task2.ts_tss.t_SS_0 = KNL_DS;
    task2.ts_tss.t_ESP_0 = (uint32_t)&ks_page[PAGE_SIZE-1];
    task2.ts_tss.t_EFLAGS = 0x200;  // NOTE: 允许中断
    task2.ts_tss.t_DS = USR_DS;
    task2.ts_tss.t_SS = USR_DS;
    task2.ts_tss.t_ESP = (uint32_t)&us_page[PAGE_SIZE-1];
    task2.ts_tss.t_CS = USR_CS;
    task2.ts_tss.t_EIP = (uint32_t)task_2;

    uint32_t *pdt = (uint32_t*)alloc_page();
    uint32_t *pte = (uint32_t*)alloc_page();
    pdt[0] = PAGE_ADDR((uint32_t)pte) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    uint32_t addr = 0;
    for (int i = 0; i < 1024; ++i) {
        pte[i] = PAGE_ADDR(addr) | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        addr += PAGE_SIZE;
    }
    task2.ts_tss.t_CR3 = (uint32_t)pdt;
    task2.ts_tss.t_LDT = KNL_LDT;
}

void
start_task()
{
    init_mutex(&one_mutex);

    cli();
    setup_idt();
    setup_gdt();

    _init_8259A();
    _init_timer();

    setup_first_task();
    setup_second_task();
    enable_paging();
    /* 开始第一个进程 */
    start_first_task(&task1.ts_tss,
                    task1.ts_ldt,
                    &task1.ts_user_stack[PAGE_SIZE-1],
                    task_1);
}
