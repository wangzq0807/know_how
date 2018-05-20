#ifndef __LOCK__
#define __LOCK__
#include "defs.h"

struct Mutex {
    uint32_t    mt_lock;
};

int init_mutex(struct Mutex *mutex);

int acquire_mutex(struct Mutex *mutex);

int release_mutex(struct Mutex *mutex);


static inline uint32_t atomic_swap(uint32_t *org, uint32_t new_val) {
    __asm__ volatile (
        "lock xchg %0, %1 \n"
        :"+m"(*org), "+r"(new_val)
    );
    return new_val;
}

#endif