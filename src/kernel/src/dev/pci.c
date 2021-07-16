#include <dev/pci.h>
#include <dev/log.h>
#include <types/dyn_array.h>
#include <io.h>

DEFINE_DYN_ARRAY(pci_device_t*, pci_devices)

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


static void pci_enumerate_function(uint8_t bus, uint8_t slot, uint8_t func){
    pci_device_t* pci_dev      = kmalloc(sizeof(pci_device_t));
    pci_dev->vendor            = PCI_VENDOR(bus, slot, func);
    pci_dev->device            = PCI_DEVICE(bus, slot, func);
    pci_dev->class_code        = PCI_CLASS(bus, slot, func);
    pci_dev->subclass_code     = PCI_SUBCLASS(bus, slot, func);
    pci_dev->bar0              = PCI_BAR0(bus, slot, func);
    pci_dev->bar1              = PCI_BAR1(bus, slot, func);
    pci_dev->bar2              = PCI_BAR2(bus, slot, func);
    pci_dev->bar3              = PCI_BAR3(bus, slot, func);
    pci_dev->bar4              = PCI_BAR4(bus, slot, func);
    pci_dev->bar5              = PCI_BAR5(bus, slot, func);
    DYN_ARRAY_ADD(pci_devices, pci_dev)
    info("PCI Bus device detected:\n\r\t%d %d %d\n\r\t0x%x 0x%x 0x%x 0x%x 0x%x IRQ: 0x%x", bus, slot, func, PCI_VENDOR(bus,slot,func), PCI_DEVICE(bus,slot,func), PCI_CLASS(bus,slot,func), PCI_SUBCLASS(bus,slot,func), PCI_HDR_TYPE(bus,slot,func), PCI_IRQ(bus,slot,func));
}

static uint8_t  pci_enumerate_device(uint8_t bus, uint8_t device){
    uint8_t function = 0;
    uint16_t vendor = PCI_VENDOR(bus, device, function);
    if(vendor == 0xFFFF) {
        return 0;        // Device doesn't exist
    }
    pci_enumerate_function(bus, device, function);
    if(PCI_IS_MULTIFUNCT(bus,device, function)) {
        for(function = 1; function < 8; function++) {
            if(PCI_VENDOR(bus, device, function) != 0xFFFF) {
                pci_enumerate_function(bus, device, function);
            }
        }
    }
}

uint8_t  pci_init(){
     INIT_DYN_ARRAY(pci_devices, 256)
     uint16_t bus;
     uint8_t device;
     for(bus = 0; bus < 256; bus++) {
         for(device = 0; device < 32; device++) {
             pci_enumerate_device(bus, device);
         }
     }
    return 0;
}