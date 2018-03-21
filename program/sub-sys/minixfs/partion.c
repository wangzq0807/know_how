#include "asm.h"
#include "partion.h"
#define PARTION_ADDR  (0x7c00+446)
#define BOOT_FLAG (0xaa55)
#define BOOT_FLAG_ADDR (0x7c00+510)

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

struct PartEntry partion_table[4];

error_t partion_load()
{
    uint16_t bootable = *((uint16_t*)BOOT_FLAG_ADDR);
    if (bootable != BOOT_FLAG)  return -1;

    memcpy(&partion_table[0],(void*)PARTION_ADDR, sizeof(struct PartEntry));
    memcpy(&partion_table[1],(void*)PARTION_ADDR, sizeof(struct PartEntry));
    memcpy(&partion_table[2],(void*)PARTION_ADDR, sizeof(struct PartEntry));
    memcpy(&partion_table[3],(void*)PARTION_ADDR, sizeof(struct PartEntry));
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