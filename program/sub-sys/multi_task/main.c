#include "task.h"
#include "log.h"
#include "asm.h"
#include "lock.h"
#include "memory.h"
#include "string.h"

void
start_main()
{
    init_memory(2*1024*1024, 4*1024*1024);
    start_task();
}
