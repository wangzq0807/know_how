#include "task.h"
#include "log.h"
#include "asm.h"
#include "lock.h"
#include "memory.h"

void
start_main()
{
    init_memory(1*1024*1024, 4*1024*1024);
    start_task();
}
