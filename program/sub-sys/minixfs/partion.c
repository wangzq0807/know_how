#include "asm.h"
#include "partion.h"
#define PARTION_ADDR  (0x7c00+446)
#define BOOT_FLAG (0xaa55)
#define BOOT_FLAG_ADDR (0x7c00+510)

struct part_entry {
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

struct part_entry partion_table[4];

int partion_load() {
    uint16_t bootable = *((uint16_t*)BOOT_FLAG_ADDR);
    if (bootable != BOOT_FLAG)  return -1;

    memcpy(&partion_table[0],(void*)PARTION_ADDR, sizeof(struct part_entry));
    memcpy(&partion_table[1],(void*)PARTION_ADDR, sizeof(struct part_entry));
    memcpy(&partion_table[2],(void*)PARTION_ADDR, sizeof(struct part_entry));
    memcpy(&partion_table[3],(void*)PARTION_ADDR, sizeof(struct part_entry));
    return 0;
}

uint32_t partion_get()
{
    return partion_table[0].lba_start;
}