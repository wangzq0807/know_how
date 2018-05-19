#ifndef __NODES_H__
#define __NODES_H__
#include "defs.h"
#include "fs.h"

struct IndexNode {
    uint16_t    in_file_mode;      // file type, auth,
    int16_t     in_num_links;      // the number of linking to this file
    int16_t     in_owner_id;       // owner's id
    int8_t      in_group_id;       // goup's id
    uint32_t    in_file_size;      // file size in types
    time_t      in_atime;          // access time
    time_t      in_mtime;          // modify time
    time_t      in_ctime;          // create time
    uint32_t    in_zones[NUMBER_ZONE];      // zone
};

struct Direction {
    uint16_t    inode;
    char        name[FILENAME_LEN];
};

error_t init_nodes(uint16_t dev);

struct IndexNode * alloc_inode(uint16_t dev);

struct IndexNode * get_inode(uint16_t dev, uint32_t idx);

error_t free_inode(struct IndexNode *inode);

uint32_t alloc_znode(uint16_t dev);

#endif // __NODES_H__