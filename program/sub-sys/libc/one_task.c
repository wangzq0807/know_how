#include "stdio.h"

int main() {
    char buf[1024] = {"asdf"};
    int fd = open("/one_task", O_RDONLY, 0);
    read(fd, buf, 1024);
    buf[0] = 'a';
    buf[4] = 0;
    write(fd, buf, 4);

    return 0;
}
