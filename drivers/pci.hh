#ifndef ARCH_X86_PCI_H
#define ARCH_X86_PCI_H

#include <stdint.h>
#include "processor.hh"
#include "types.hh"

class Driver;

namespace pci {

using processor::inb;
using processor::inw;
using processor::inl;
using processor::outb;
using processor::outw;
using processor::outl;

	enum pc_early_defines {
	    PCI_CONFIG_ADDRESS = 0xcf8,
	    PCI_CONFIG_DATA    = 0xcfc,
	    PCI_BUS_OFFSET     = 16,
	    PCI_SLOT_OFFSET    = 11,
	    PCI_FUNC_OFFSET    = 8,
	    PCI_CONFIG_ADDRESS_ENABLE = 0x80000000,
	    PCI_COMMAND_OFFSET = 0x4,
	    PCI_BUS_MASTER_BIT = 0x2,
	    PCI_STATUS_OFFSET  = 0x6,
	    PCI_CLASS_REVISION = 0x8,
	    PCI_CLASS_OFFSET   = 0xb,
	    PCI_SUBCLASS_OFFSET= 0xa,
	    PCI_HEADER_TYPE    = 0xe,
	    PCI_SUBSYSTEM_ID   = 0x2e,
	    PCI_SUBSYSTEM_VID  = 0x2c,
	    PCI_HEADER_MULTI_FUNC = 0x80,
	    PCI_BAR0_ADDR      = 0x10,
	    PCI_CAPABILITIES_PTR = 0x34,
	};

	u32 read_pci_config(u8 bus, u8 slot, u8 func, u8 offset);
    u16 read_pci_config_word(u8 bus, u8 slot, u8 func, u8 offset);
	u8 read_pci_config_byte(u8 bus, u8 slot, u8 func, u8 offset);
	void write_pci_config(u8 bus, u8 slot, u8 func, u8 offset, u32 val);
    void write_pci_config_word(u8 bus, u8 slot, u8 func, u8 offset, u16 val);
	void write_pci_config_byte(u8 bus, u8 slot, u8 func, u8 offset, u8 val);
	void pci_device_print(u8 bus, u8 slot, u8 func);
	void pci_devices_print(void);
	void pci_device_enumeration(void);

	class Bar {
	public:
	    Bar(int n, Driver* d);
	    enum TYPE {
	      BAR_TYPE = 1,
	      BAR_IO = 0,
	      BAR_MMIO = 1,
	    };

	    u32 read(u32 offset)  {return inl(_addr+offset);}
	    u16 readw(u32 offset) {return inw(_addr+offset);}
	    u8  readb(u32 offset) {return inb(_addr+offset);}
	    void write(u32 offset, u32 val) {outl(val, _addr+offset);}
	    void write(u32 offset, u16 val) {outw(val, _addr+offset);}
	    void write(u32 offset, u8 val)  {outb(val, _addr+offset);}

	private:
	    u32 _addr;
	    TYPE _type;
	};

};

#endif
