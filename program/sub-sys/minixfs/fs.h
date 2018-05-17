#ifndef __FS_H__
#define __FS_H__

#define ROOT_DEVICE    0

#define SECTOR_SIZE             512         // 一个扇区大小
#define PER_BLOCK_SECTORS       2           // 每块对应的扇区数
#define SUPER_BLOCK_BEGAIN      1           // 超级块在分区中的开始块号
#define SUPER_BLOCK_SIZE        1           // 超级块的块数
#define MINIX_V2                0x2478      // minix v2文件系统标识

#define MAX_IMAP_NUM            8           // inode位图最大区块数
#define MAX_ZMAP_NUM            8           // znode位图最大区块数

#endif // __FS_H__