#include "vga.h"
#include "defs.h"

void
start_main()
{
    set_color(COL_BLUE, COL_GRAY);
    for (int i = 0; i < 24; ++i)
        print("abcd\n");
    print("asdf\na\nasdfasdf\naa");
    print("asdf\na\nasdfasdf\naa");
    print("\b\b\b");
    // printx(get_pos());
    // update_cursor(4, 0);
}