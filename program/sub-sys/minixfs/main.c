#include "defs.h"
#include "asm.h"

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

void lba_data_trans(
    uint32_t lba_addr, uint32_t buf, uint16_t cnt,
    uint32_t cmd )
{
    // read ata disk by LBA-28bit-mode
    lba_addr = lba_addr & 0xffffff;             // 28bit
    const uint8_t drv_mode = 0xE0;              // use master hd and use LBA mode
    const uint8_t use_dma = 0;                  // user DMA mode

    outb(drv_mode|BYTE4(lba_addr) , ATA_REG_DRV);  // select driver and set LBA mode
    // TODO: wait for 400ns
    outb(use_dma, ATA_REG_FEATURES);            // use DMA mode
    outb(cnt, ATA_REG_COUNT);                   // sector count
    outb(BYTE1(lba_addr), ATA_REG_LBA1);        // lba address  0-7
    outb(BYTE2(lba_addr), ATA_REG_LBA2);        // lba address  8-15
    outb(BYTE3(lba_addr), ATA_REG_LBA3);        // lba address  16-23
    outb(cmd, ATA_REG_CMD);                     // begin command
}

void start_main()
{
    lba_data_trans(2, 2, 1, ATA_CMD_READ);
}