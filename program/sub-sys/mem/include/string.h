#ifndef __STRING_H__
#define __STRING_H__
#include "defs.h"

static inline void memcpy(void *dest, const void *src, size_t size) {
    while (size--) {
        ((char*)dest)[size] = ((char*)src)[size];
    }
}

static inline void memset(void *dest, int value, size_t size) {
    while (size--) {
        ((char*)dest)[size] = value & 0XFF;
    }
}

static inline int strlen(const char *str) {
    int ret = 0;
    for (; str[ret]; ret++) ;
    return ret;
}

static inline int strcmp(const char *s1, const char *s2) {
    while ( *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    int diff = s1[0] - s2[0];
    if (diff > 0)
        return 1;
    else if(diff < 0)
        return -1;
    else
        return 0;
}

#endif