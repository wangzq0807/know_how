#include "asm.h"
#include "partion.h"
#include "hard_disk.h"
#include "string.h"

#define PARTION_POS (446)
#define BOOT_FLAG (0xaa55)
#define BOOT_FLAG_POS (510)

struct PartEntry {
    uint8_t indicator;
    uint8_t start_header;
    uint8_t start_sector;
    uint8_t start_cylinder;
    uint8_t system_id;
    uint8_t end_header;
    uint8_t end_sector;
    uint8_t end_cylinder;
    uint32_t lba_start;
    uint32_t total_sector;
};

uint8_t buffer[512] = { 0 };
struct PartEntry partion_table[4];

error_t partion_load()
{
    ata_read(0, 1, buffer);
    uint16_t bootable = *(uint16_t*)(buffer + BOOT_FLAG_POS);
    if (bootable != BOOT_FLAG)  return -1;

    memcpy(&partion_table[0], buffer + PARTION_POS, sizeof(struct PartEntry));
    memcpy(&partion_table[1], buffer + PARTION_POS, sizeof(struct PartEntry));
    memcpy(&partion_table[2], buffer + PARTION_POS, sizeof(struct PartEntry));
    memcpy(&partion_table[3], buffer + PARTION_POS, sizeof(struct PartEntry));
    return 0;
}

uint32_t partion_get_active()
{
    for (int i = 0; i < 4; ++i) {
        if (partion_table[0].indicator & 0x80)
            return i;
    }
    for (int i = 0; i < 4; ++i) {
        if (partion_table[0].total_sector)
            return i;
    }
    return 0xffff;
}

uint32_t partion_get_start(uint32_t i)
{
    return partion_table[i].lba_start;
}

uint32_t partion_get_totalsector(uint32_t i)
{
    return partion_table[i].total_sector;
}