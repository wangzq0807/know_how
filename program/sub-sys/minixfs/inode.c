#include "inode.h"

struct IndexNode {
    uint16_t    file_mode;      // file type, auth,
    int16_t     num_links;      // the number of linking to this file
    int16_t     owner_id;       // owner's id
    int8_t      group_id;       // goup's id
    uint32_t    file_size;      // file size in types
    time_t      atime;          // access time
    time_t      mtime;          // modify time
    time_t      ctime;          // create time
    uint32_t    zones[10]       // zone
};