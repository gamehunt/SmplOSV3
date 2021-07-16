#ifndef K_SMPLOS_PCI_H
#define K_SMPLOS_PCI_H

#include <stdint.h>

#if defined(K_SMPLOS_KERNEL)
    #include <util.h>
#else
    #include <kernel/util.h>
#endif
#define PCI_VENDOR(bus, slot, func)        (pci_config_read_word(bus, slot, func, 0x0))
#define PCI_DEVICE(bus, slot, func)        (pci_config_read_word(bus, slot, func, 0x2))
#define PCI_CLASS(bus, slot, func)         (pci_config_read_word(bus, slot, func, 0xB)  >> 8)
#define PCI_SUBCLASS(bus, slot, func)      (pci_config_read_word(bus, slot, func, 0xB)  & 0xFF)
#define PCI_HDR_TYPE(bus, slot, func)      (pci_config_read_word(bus, slot, func, 0xE)  & 0x7F)
#define PCI_IS_MULTIFUNCT(bus, slot, func) (pci_config_read_word(bus, slot, func, 0xE)  & 0x80)
#define PCI_IRQ(bus, slot, func)           (pci_config_read_word(bus, slot, func, 0x3C) & 0xFF)

#define PCI_BAR0(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x10) | (pci_config_read_word(bus, slot, func, 0x12) << 16))
#define PCI_BAR1(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x14) | (pci_config_read_word(bus, slot, func, 0x16) << 16))
#define PCI_BAR2(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x18) | (pci_config_read_word(bus, slot, func, 0x12) << 16))
#define PCI_BAR3(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x1C) | (pci_config_read_word(bus, slot, func, 0x1A) << 16))
#define PCI_BAR4(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x20) | (pci_config_read_word(bus, slot, func, 0x22) << 16))
#define PCI_BAR5(bus, slot, func)          ((uint32_t)pci_config_read_word(bus, slot, func, 0x24) | (pci_config_read_word(bus, slot, func, 0x26) << 16))


typedef struct{
    uint16_t vendor;
    uint16_t device;
    uint8_t  class_code;
    uint8_t  subclass_code;
    uint8_t  header_type;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint8_t irq;  
}pci_device_t;

CH_START

pci_device_t* pci_seek_device(uint8_t vendor, uint8_t device);
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint8_t  pci_init();

CH_END

#endif