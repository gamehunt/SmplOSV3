#include <stdint.h>
#include <stdio.h>

#include <kernel/module.h>
#include <kernel/dev/log.h>
#include <kernel/io.h>
#include <kernel/types/registers.h>

#include <ata.h>

uint8_t load();
uint8_t unload();

SMPLOS_MODULE("ata", load, unload)

static ata_device_t* ata_devices[4];

uint16_t ata_get_iobase(ata_device_t* dev){
    return dev->bus ? ATA_SECONDARY_IO_BASE : ATA_PRIMARY_IO_BASE;
}

uint16_t ata_get_ctrlbase(ata_device_t* dev){
    return dev->bus ? ATA_SECONDARY_CONTROL_BASE : ATA_PRIMARY_CONTROL_BASE;
}

void ata_cache_flush(ata_device_t* dev){
    uint16_t base = ata_get_iobase(dev);
    outb(ATA_COMMAND(base), 0xE7);
}

void ata_sel_drive(ata_device_t* dev){
	uint16_t base = ata_get_iobase(dev);
	outb(ATA_DRIVE_SEL(base),dev->slave ? 0xB0 : 0xA0);
//	selected_drive = ata_index(bus,drive);
}

uint8_t ata_identity(uint8_t secondary, uint8_t slave, uint16_t* buffer){
    uint16_t iobase = secondary ? ATA_SECONDARY_IO_BASE : ATA_PRIMARY_IO_BASE;

    outb(ATA_DRIVE_SEL(iobase), slave ? 0xB0 : 0xA0);
    outb(ATA_SECCOUNT(iobase),    0);
    outb(ATA_SECNUM_LOW(iobase),  0);
    outb(ATA_SECNUM_MID(iobase),  0);
    outb(ATA_SECNUM_HIGH(iobase), 0);
    outb(ATA_COMMAND(iobase),   0xEC);
    
    if(inb(ATA_STATUS(iobase))){
      while(inb(ATA_STATUS(iobase)) & 0x80){;}
      
      if(inb(ATA_SECNUM_MID(iobase)) || inb(ATA_SECNUM_HIGH(iobase))){
          return 0;
      }
      
      while(1){
          uint8_t status = inb(ATA_STATUS(iobase));
          if(status & 1){
              return 1;
          }else if(status & 8){
              for(int i=0;i<256;i++){
                  buffer[i] = inw(ATA_DATA(iobase));
              }
              return 0;
          }
      }
    }else{
        return 2;
    }
}

void ata_irq_handler_master(regs_t registers){
    info("ATA MASTER IRQ");
    uint8_t status = inb(ATA_STATUS(ATA_PRIMARY_IO_BASE));
    /*uint8_t buffer[512];
    for(int i=0;i<256;i++){
        ((uint16_t*)buffer)[i] = inw(ATA_DATA(ATA_PRIMARY_IO_BASE));
    }
    info("%b %s", status, &buffer[440]);*/
}

void ata_irq_handler_slave(regs_t registers){
    info("ATA SLAVE IRQ");
    uint8_t status = inb(ATA_STATUS(ATA_SECONDARY_IO_BASE));
    /*for(int i=0;i<256;i++){
        inw(ATA_DATA(ATA_SECONDARY_IO_BASE));
    }*/
}

uint16_t ata_read_sectors(ata_device_t* dev, uint64_t lba, uint16_t sec_count){
    //ata_sel_drive(dev);
    uint16_t iobase = ata_get_iobase(dev);
    if(dev->lba48){
        outb(ATA_DRIVE_SEL(iobase), 0x40 | (dev->slave << 4));
        outb(ATA_SECCOUNT(iobase), sec_count >> 8);
        outb(ATA_SECNUM_LOW(iobase),  (uint8_t) (lba >> 24));
        outb(ATA_SECNUM_MID(iobase),  (uint8_t) (lba >> 32));
        outb(ATA_SECNUM_HIGH(iobase), (uint8_t) (lba >> 40));
        outb(ATA_SECCOUNT(iobase), sec_count & 0xFF);
        outb(ATA_SECNUM_LOW(iobase),  (uint8_t) (lba));
        outb(ATA_SECNUM_MID(iobase),  (uint8_t) (lba >> 8));
        outb(ATA_SECNUM_HIGH(iobase), (uint8_t) (lba >> 16));
        outb(ATA_COMMAND(iobase), 0x24);
    }else{
        uint32_t slba = (uint32_t)lba;

        outb(ATA_FEATURES(iobase), 0x00);
        outb(ATA_DRIVE_SEL(iobase), 0xE0 | (!dev->slave << 4) | (slba >> 24) & 0x0F);
        outb(ATA_SECCOUNT(iobase), sec_count);
        outb(ATA_SECNUM_LOW(iobase),  (uint8_t) (slba));
        outb(ATA_SECNUM_MID(iobase),  (uint8_t) (slba >> 8));
        outb(ATA_SECNUM_HIGH(iobase), (uint8_t) (slba >> 16));
        outb(ATA_COMMAND(iobase), 0x20);
    }
    ata_cache_flush(dev);
    return sec_count;
}

void ata_reset(uint8_t slave){
	uint16_t base = slave ? ATA_SECONDARY_CONTROL_BASE : ATA_PRIMARY_CONTROL_BASE;
    outb(ATA_DEVICE_CTRL(base),0x20);
	inb(ATA_ALT_STATUS(base));
	outb(ATA_DEVICE_CTRL(base),0x00);
}

uint8_t unload(){
    return 0;
}

uint8_t load()
{

    ata_reset(0);
    ata_reset(1);

    memset(ata_devices, 0, sizeof(ata_device_t*)*4);

    uint16_t buffer[256];
    for(uint8_t i=0;i<=1;i++){
        for(uint8_t j=0;j<=1;j++){
            if(!ata_identity(i,j, buffer)){

                ata_device_t* ata_device = kmalloc(sizeof(ata_device_t));
                
                ata_device->bus   = i;
                ata_device->slave = j;

                ata_device->lba48 = (buffer[83] & 0x400) ? 1 : 0;
                
                ata_device->lba28_sectors = *((uint32_t*)&buffer[60]);
                if(ata_device->lba48){
                    ata_device->lba48_sectors = *((uint64_t*)&buffer[100]);
                }

                ata_device->dma_available = (uint8_t)(buffer[88] & 0xFF);
                ata_device->dma_active    = (uint8_t)((buffer[88] & 0xFF00) >> 8);

                ata_device->ident_buffer = kmalloc(sizeof(uint16_t)*256);
                memcpy(ata_device->ident_buffer, buffer, sizeof(uint16_t)*256);

                ata_devices[2*i + j] = ata_device;

                outb(ATA_DRIVE_ADDR(ata_get_ctrlbase(ata_device)), 0);

                info("Found ATA device: at %s (%s) - %s - %d sectors;\n\r DMA: ACT: %b AVAIL: %b", i ? "secondary bus" : "primary bus", 
                                                                                   j ? "slave" : "master",
                                                                                   ata_device->lba48 ? "LBA48" : "LBA28", 
                                                                                   ata_device->lba48 ? ata_device->lba48_sectors : ata_device->lba28_sectors, 
                                                                                   ata_device->dma_active, 
                                                                                   ata_device->dma_available);
            }
        }
    }

    set_irq_handler(14, &ata_irq_handler_master);
    set_irq_handler(15, &ata_irq_handler_slave );

    //ata_read_sectors(ata_devices[0], 0, 1);

    return 0;
}