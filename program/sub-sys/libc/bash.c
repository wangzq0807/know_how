#include "unistd.h"
#include "stdio.h"

int
main(int argc, const char **argv)
{
    char buf[1025] = {"asdf\n"};
    int fd = 0;
    fd = open("/dev/tty", O_RDWR, 0);
    if (fd == -1)
        return 0;

    while (1) {
        int cnt = read(fd, buf, 1024);
        if (cnt == -1 || cnt == 0)
            continue;
        if (buf[cnt-1] == '\r') {
            buf[cnt] = 0;
        }

        int pid = fork();
        if (pid == 0) {
            char *params[] = {"Hello,World!", NULL};
            execve("/echo", params, NULL);
        }
        // write(fd, buf, cnt);
    }
    close(fd);

    return 0;
}
