#ifndef __INODE__
#define __INODE__
#include "defs.h"
#include "fs.h"

error_t inode_ls(uint32_t inode);

error_t inode_new(uint32_t parent, const char* filename, uint16_t file_mode);

#endif // __INODE__