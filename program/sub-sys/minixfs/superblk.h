#ifndef __SUPERBLK__
#define __SUPERBLK__
#include "defs.h"

error_t superblk_load(uint32_t lba_addr);

uint32_t superblk_get_imap_blocks();

uint32_t superblk_get_zmap_blocks();

#endif // __SUPERBLK__