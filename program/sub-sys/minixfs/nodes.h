#ifndef __NODES_H__
#define __NODES_H__
#include "defs.h"

error_t init_nodes(uint16_t dev);

uint32_t alloc_inode(uint16_t dev);

uint32_t alloc_znode(uint16_t dev);

#endif // __NODES_H__