#include "unistd.h"

extern int main();

int _start() {
    int r = main();
    exit(r);
    return 0;
}
