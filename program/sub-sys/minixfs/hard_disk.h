#ifndef __HARD_DISK__
#define __HARD_DISK__
#include "defs.h"
// ATA registers(Primary Bus)
#define ATA_REG_DATA        0x1F0
#define ATA_REG_FEATURES    0x1F1
#define ATA_REG_COUNT       0x1F2
#define ATA_REG_LBA1        0x1F3
#define ATA_REG_LBA2        0x1F4
#define ATA_REG_LBA3        0X1F5
#define ATA_REG_DRV         0x1F6
#define ATA_REG_CMD         0x1F7
#define ATA_REG_STATUS      0x3F6
// ATA command set
#define ATA_CMD_READ    0x20    // read command
#define ATA_CMD_WRITE   0x30    // write command
// ATA status
#define ATA_STATUS_ERR      0x1
#define ATA_STATUS_INDEX    0x2
#define ATA_STATUS_ECC      0x4
#define ATA_STATUS_DRQ      0x8
#define ATA_STATUS_SEEK     0x10
#define ATA_STATUS_FAULT    0x20
#define ATA_STATUS_READY    0x40
#define ATA_STATUS_BUSY     0x80

#define IDE0_MAJOR          3
#define HD_MAJOR            IDE0_MAJOR

typedef     uint32_t    lba_t;

// 检查驱动器是否能接收命令
int ata_is_ready();
// 发送命令
int ata_cmd(lba_t lba_addr, uint8_t cnt, uint8_t cmd);
// 读磁盘
int ata_read(lba_t lba_addr, uint8_t cnt, void *buffer);
// 写磁盘
int ata_write(const void *buffer, lba_t lba_addr, uint8_t cnt);

#endif