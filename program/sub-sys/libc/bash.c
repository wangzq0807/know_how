#include "unistd.h"
#include "stdio.h"
#include "string.h"

#define HOSTNAME    ("smash$")

int
main(int argc, const char **argv)
{
    char buf[1025] = {0};
    int fd = 0;
    fd = open("/dev/tty", O_RDWR, 0);
    if (fd == -1)
        return 0;

    printf(HOSTNAME);
    while (1) {
        int cnt = read(fd, buf, 1024);
        if (cnt == -1 || cnt == 0)
            continue;
        if (buf[cnt-1] == '\r') {
            buf[cnt] = 0;
        }

        if (strncmp(buf, "cd ", 3) == 0) {
            int dirstart = 3;
            buf[cnt-1] = 0;
            for (; buf[dirstart] != 0 && buf[dirstart] == ' '; ++dirstart);
            int r = chdir(buf+dirstart);
            if (r == -1) {
                printf("failed to open target dir\n");
            }
            printf(HOSTNAME);
            continue;
        }

        int pid = fork();
        if (pid == 0) {
            char *params[10] = { NULL };
            char *tmp = buf;
            for (int i = 0; i < 10; ++i) {
                char *token = strsep(&tmp, " \r");
                if (token == NULL)  break;
                params[i] = token;
            }

            if (execve(params[0], params+1, NULL) == -1) {
                printf("exec %s failed\n", params[0]);
                exit(0);
            }
        }
        else if (pid > 0) {
            waitpid(pid, NULL, 0);
            printf(HOSTNAME);
        }
    }
    close(fd);

    return 0;
}
