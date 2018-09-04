#include "vga.h"
#include "defs.h"

void
start_main()
{
    print("abcd");
    update_cursor(4, 0);
}