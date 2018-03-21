#ifndef __BITMAP__
#define __BITMAP__
#include "defs.h"

error_t bitmap_inode_load(uint32_t lba_start, uint32_t cnt);

error_t bitmap_znode_load(uint32_t lba_start, uint32_t cnt);

#endif // __BITMAP__