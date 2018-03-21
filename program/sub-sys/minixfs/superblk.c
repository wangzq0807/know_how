#include "superblk.h"
#include "hard_disk.h"

uint8_t buffer[512] = {0};

struct SuperBlock {
    uint16_t    inodes;                 // inode number
    uint16_t    dev_size;               // device size
    int16_t     imap_blocks;            // blocks need by inode map
    int16_t     zmap_blocks;            // blocks need by zone map
    uint16_t    first_datazone;         // first block number of datazone
    int16_t     log_zone_size;          // log2 of block/zone
    uint32_t    max_file_size;          // max file size
    int16_t     magic;                  // magic
    int16_t     zones;                  // zone number
};

error_t superblk_load(uint32_t lba_addr) {
    ata_read(lba_addr, 1, &buffer);

    return 0;
}