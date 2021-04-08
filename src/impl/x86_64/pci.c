#include "pci.h"
#include "io.h"
#include "print.h"

#define ones(n_bits) ((unsigned int)((1 << (n_bits)) - 1))
#define bit_test(bit) ((unsigned int)(1 << ((bit) - 1)))

#define BYTE_BITS 8

uint32_t pci_read_config(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
	outd(PCI_CONFIG_ADDRESS,
		  (((uint32_t)bus & ones(8)) << 16)
		| (((uint32_t)device & ones(5)) << 11)
		| (((uint32_t)func & ones(3)) << 8)
		| (((uint32_t)offset & ones(6)) << 2)
		| (1 << 31) // enable bit
	);
	return ind(PCI_CONFIG_DATA);
}

bool pci_find_device_by_type(struct pci_device_index*const out, const uint8_t class, const uint8_t subclass, const uint8_t prog_if) {
	for (uint16_t bus = 0; bus < (1 << 8); bus++) {
		for (uint8_t slot = 0; slot < (1 << 5); slot++) {
			for (uint8_t func = 0; func < (1 << 3); func++) {
				const uint32_t result = pci_read_config(bus, slot, func, 2);
				if (result == 0xffffffff) continue;
				if (
					((result >> 24) & ones(8)) == class
					&& ((result >> 16) & ones(8)) == subclass
					&& ((result >> 8) & ones(8)) == prog_if
				) {
					*out = (struct pci_device_index) { bus, slot, func };
					return true;
				}
			}
		}
	}
	return false;
}
