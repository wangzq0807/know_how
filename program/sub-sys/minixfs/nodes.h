#ifndef __NODES_H__
#define __NODES_H__
#include "defs.h"
#include "fs.h"

struct PyIndexNode {
    uint16_t    in_file_mode;      // 文件类型, 权限等,
    int16_t     in_num_links;      // 链接到这个文件的数量
    int16_t     in_owner_id;       // 拥有者的id
    int8_t      in_group_id;       // 所在组的id
    uint32_t    in_file_size;      // 文件大小 in types
    time_t      in_atime;          // 访问时间
    time_t      in_mtime;          // 修改时间
    time_t      in_ctime;          // 创建时间
    uint32_t    in_zones[NUMBER_ZONE];      // 区块
};

struct IndexNode {
    struct PyIndexNode  in_inode;
    uint32_t            in_inum;
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

void * get_znode(uint16_t dev, uint32_t idx);

#endif // __NODES_H__