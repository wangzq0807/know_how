#ifndef __INODE__
#define __INODE__
#include "defs.h"

#define S_IXOTH     0000001         // executable for other
#define S_IWOTH     0000002         // readable for other
#define S_IROTH     0000004         // writable for other
#define S_IRWXO     0000007         // rwx for other

#define S_IXGRP     0000010         // executable for group
#define S_IWGRP     0000020         // readable for group
#define S_IRGRP     0000040         // writable for group
#define S_IRWXG     0000070         // rwx for group

#define S_IXUSR     0000100         // executable for owner
#define S_IWUSR     0000200         // readable for owner
#define S_IRUSR     0000400         // writable for owner
#define S_IRWXU     0000700         // rwx for owner

#define S_IFIFO     0010000         // fifo file
#define S_IFCHR     0020000         // char file
#define S_IFDIR     0040000         // dir file
#define S_IFBLK     0060000         // block file
#define S_IFREG     0100000         // regular file
#define S_IFLNK     0120000         // symbolic file
#define S_IFSOCK    0140000         // socket file

#define S_IFMT      0170000         // mask of file type
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

error_t inode_load(uint32_t node_blk, uint32_t i);

error_t inode_root_ls(uint32_t first);

#endif // __INODE__