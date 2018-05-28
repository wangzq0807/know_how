#ifndef __NODES_H__
#define __NODES_H__
#include "defs.h"
#include "fs.h"

#define INODE_LOCK        1

struct PyIndexNode {
    uint16_t    in_file_mode;      // 文件类型, 权限等,
    int16_t     in_num_links;      // 链接到这个文件的数量
    int16_t     in_owner_id;       // 拥有者的id
    int8_t      in_group_id;       // 所在组的id
    uint32_t    in_file_size;      // 文件大小 in types
    time_t      in_atime;          // 访问时间
    time_t      in_mtime;          // 文件修改时间
    time_t      in_ctime;          // Inode修改时间
    uint32_t    in_zones[NUMBER_ZONE];      // 区块
};

struct IndexNode {
    struct PyIndexNode  in_inode;       // 磁盘上inode内容
    uint16_t            in_dev;         // 设备号
    uint32_t            in_status;      // 状态
    uint16_t            in_inum;        // inode编号
    uint16_t            in_refs;        // 引用计数
    struct IndexNode    *in_next;       // 下一个空闲inode
    struct IndexNode    *in_hash_prev;  // hash表
    struct IndexNode    *in_hash_next;
};

struct Direction {
    uint16_t    inode;
    char        name[FILENAME_LEN];
};

error_t
init_nodes(uint16_t dev);

struct IndexNode *
alloc_inode(uint16_t dev);

error_t
free_inode(struct IndexNode *inode);

struct IndexNode *
get_inode(uint16_t dev, uint16_t idx);

void
release_inode(struct IndexNode *inode);

uint32_t
alloc_zone(uint16_t dev);

uint32_t
get_znode(struct IndexNode *inode, uint32_t bytes_offset, uint32_t *offset_in_blk);

#endif // __NODES_H__