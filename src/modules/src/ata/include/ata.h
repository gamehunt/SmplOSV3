#ifndef SMPLOS_ATA_MODULE_H
#define SMPLOS_ATA_MODULE_H

#include <kernel/util.h>

#define ATA_PRIMARY_IO_BASE        0x1F0
#define ATA_PRIMARY_CONTROL_BASE   0x3F6

#define ATA_SECONDARY_IO_BASE      0x170
#define ATA_SECONDARY_CONTROL_BASE 0x376

#define ATA_DATA(io_base)         (io_base)     //RW 16/16
#define ATA_ERROR(io_base)        (io_base) + 1 //R  8/16
#define ATA_FEATURES(io_base)     (io_base) + 1 //W  8/16
#define ATA_SECCOUNT(io_base)     (io_base) + 2 //RW 8/16
#define ATA_SECNUM_LOW(io_base)   (io_base) + 3 //RW 8/16
#define ATA_SECNUM_MID(io_base)   (io_base) + 4 //RW 8/16
#define ATA_SECNUM_HIGH(io_base)  (io_base) + 5 //RW 8/16
#define ATA_DRIVE_SEL(io_base)    (io_base) + 6 //RW 8/8
#define ATA_STATUS(io_base)       (io_base) + 7 //R  8/8
#define ATA_COMMAND(io_base)      (io_base) + 7 //W  8/8


#define ATA_ALT_STATUS(ctrl_base)    (ctrl_base)     //R  8/8
#define ATA_DEVICE_CTRL(ctrl_base)   (ctrl_base)     //W  8/8
#define ATA_DRIVE_ADDR(ctrl_base)    (ctrl_base) + 1 //R  8/8

#define ATA_ERR_AMNF  0
#define ATA_ERR_TKZNF 1
#define ATA_ERR_ABRT  2
#define ATA_ERR_MCR   3
#define ATA_ERR_IDNF  4
#define ATA_ERR_MC    5
#define ATA_ERR_UNC   6
#define ATA_ERR_BBK   7

typedef struct{
    uint8_t bus;
    uint8_t slave;
    uint8_t lba48;
    uint8_t dma_active;
    uint8_t dma_available;
    uint16_t* ident_buffer;
    uint32_t  lba28_sectors;
    uint64_t  lba48_sectors;
}ata_device_t;

CH_START

CH_END

#endif