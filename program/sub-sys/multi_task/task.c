#include "task.h"
#include "log.h"
#include "asm.h"
#include "lock.h"


/* 任务一 */
struct X86TSS tss1 = { 0 };
uint8_t tss1_kernal_stack[1024] = { 0 };
uint8_t tss1_user_stack[1024] = { 0 };
struct X86Desc tss1_ldt[3] = { 0 };
/* 任务二 */
struct X86TSS tss2 = { 0 };
uint8_t tss2_kernal_stack[1024] = { 0 };
uint8_t tss2_user_stack[1024] = { 0 };
struct X86Desc tss2_ldt[3] = { 0 };

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

static void
setup_ldt()
{
    const uint8_t USR_DPL = 0xF;
    setup_code_desc(&tss1_ldt[1], 0, 0xFFFFF, USR_DPL);
    setup_data_desc(&tss1_ldt[2], 0, 0xFFFFF, USR_DPL);
    setup_code_desc(&tss2_ldt[1], 0, 0xFFFFF, USR_DPL);
    setup_data_desc(&tss2_ldt[2], 0, 0xFFFFF, USR_DPL);
}

static void
setup_tss()
{
    tss1.t_SS_0 = KNL_DS;
    tss1.t_ESP_0 = (uint32_t)&tss1_kernal_stack[1023];
    tss1.t_EIP = (uint32_t)task_1;
    tss1.t_LDT = (uint32_t)0x20;

    tss2.t_SS_0 = KNL_DS;
    tss2.t_ESP_0 = (uint32_t)&tss2_kernal_stack[1023];

    tss2.t_EFLAGS = 0x200;  // NOTE: 允许中断
    tss2.t_DS = 0x17;
    tss2.t_SS = 0x17;
    tss2.t_ESP = (uint32_t)&tss2_user_stack[1023];
    tss2.t_CS = 0xF;
    tss2.t_EIP = (uint32_t)task_2;
    tss2.t_LDT = (uint32_t)0x30;
}

void
switch_task()
{
    // NOTE：task1经过跳转到task2，task2再跳转回task1时，会从ljmp的下一条指令开始执行。
    // NOTE：下面这种切换方式是抢占式的，单核情况下，当两个线程需要同步时，加锁必须是原子操作
    // NOTE：对于非抢占式的内核，加锁无需原子操作
    if (current == 1) {
        current = 2;
        switch_tss(&tss2, tss2_ldt);
    }
    else {
        current = 1;
        switch_tss(&tss1, tss1_ldt);
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
    __asm__ volatile (
        "mov $0x17, %%ax \n"
        "mov %%ax, %%ds \n"
        : : : "eax"
    );

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


void
start_task()
{
    init_mutex(&one_mutex);

    cli();
    setup_idt();
    setup_gdt();
    setup_ldt();
    setup_tss();

    _init_8259A();
    _init_timer();

    /* 开始第一个进程 */
    start_first_task(&tss1, tss1_ldt, &tss1_user_stack[1023], task_1);
}
