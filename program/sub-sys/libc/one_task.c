#include "sys/syscalls.h"

int main() {
    while (1) {
        open("/2M", 123, 456);
        // 延时
        int cnt = 100000;
        while(cnt--);
    }
    return 0;
}
