#include "path.h"
#include "string.h"
#include "inodes.h"
#include "zones.h"
#include "sys/stat.h"
#include "buffer.h"
#include "log.h"
#include "asm.h"

uint16_t cur_inode = ROOT_INODE;
uint16_t cur_dev = ROOT_DEVICE;

uint32_t
_next_file(struct IndexNode *inode, uint32_t next, struct Direction *dir)
{
    uint32_t offset = 0;
    uint32_t zone = get_zone(inode, next, &offset);
    // TODO: zone和block
    struct BlockBuffer *blk = get_block(inode->in_dev, zone);
    memcpy(dir, blk->bf_data + offset, sizeof(struct Direction));
    release_block(blk);

    return next + sizeof(struct Direction);
}

static uint32_t
_search_file(struct IndexNode *inode, const char *name, uint32_t len)
{
    if (S_ISDIR(inode->in_inode.in_file_mode)) {
        uint32_t seek = 0;
        while (seek < inode->in_inode.in_file_size) {
            struct Direction dir;
            seek = _next_file(inode, seek, &dir);
            if (strncmp(name, dir.dr_name, len) == 0) {
                return dir.dr_inode;
            }
        }
    }
    // TODO: 非法的zone号
    return 0;
}

uint32_t
name_to_inode(const char *name)
{
    uint16_t work_inode = ROOT_INODE;
    uint16_t work_dev = ROOT_DEVICE;
    if (name[0] == '/') {
        work_inode = ROOT_INODE;
        work_dev = ROOT_DEVICE;
        name += 1;
    }
    else {
        work_inode = cur_inode;
        work_dev = cur_dev;
    }

    while (*name) {
        const char *next = strstr(name, "/");
        const int len = next - name;

        struct IndexNode *inode = get_inode(work_dev, work_inode);
        work_inode = _search_file(inode, name, len);
        // TODO: 不确定
        work_dev = inode->in_dev;
        release_inode(inode);

        if (*next == '/')
            name = next + 1;
        else if (*next == 0)
            name = next;
    }

    return work_inode;
}