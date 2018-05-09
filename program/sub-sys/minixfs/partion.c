#include "asm.h"
#include "partion.h"
#include "hard_disk.h"
#include "string.h"
#include "memory.h"

#define PARTION_NUM (4)
#define PARTION_POS (446)
#define BOOT_FLAG (0xaa55)
#define BOOT_FLAG_POS (510)
#define ACTIVE_FLAG (0x80)

struct PartionEntity partion_table[PARTION_NUM];

error_t
init_partion()
{
    // 获取磁盘上第一个块
    struct BlockBuffer *buffer = get_block(1, 0);

    uint16_t bootable = *(uint16_t*)(buffer->bf_data + BOOT_FLAG_POS);
    if (bootable != BOOT_FLAG)  return -1;

    memcpy((void*)partion_table, buffer->bf_data + PARTION_POS, PARTION_NUM*sizeof(struct PartionEntity));

    release_block(buffer);
    return 0;
}

struct PartionEntity *
get_partion_entity(uint32_t i)
{
    return &partion_table[i];
}
