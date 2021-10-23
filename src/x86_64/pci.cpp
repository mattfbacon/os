#include "bitops.hpp"
#include "io.hpp"
#include "pci.hpp"
#include "print.hpp"

uint32_t pci_read_config(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
	outd(
		PCI_CONFIG_ADDRESS,
		(((uint32_t)bus & ones(8)) << 16) | (((uint32_t)device & ones(5)) << 11) | (((uint32_t)func & ones(3)) << 8) | (((uint32_t)offset & ones(6)) << 2) | (1 << 31)  // enable bit
	);
	return ind(PCI_CONFIG_DATA);
}

bool pci_find_device_by_type(struct pci_device_index* const out, const uint8_t class_, const uint8_t subclass) {
	for (uint16_t bus = out->bus; bus < ((uint16_t)1 << 8); bus++) {
		for (uint8_t slot = out->slot; slot < ((uint8_t)1 << 5); slot++) {
			for (uint8_t func = out->function; func < ((uint8_t)1 << 3); func++) {
				auto const result = pci_read_config(bus, slot, func, 2);
				if (result == 0xffffffff) continue;
				if (((result >> 24) & ones(8)) == class_ && ((result >> 16) & ones(8)) == subclass) {
					*out = { (uint8_t)bus, slot, func };
					return true;
				}
			}
		}
	}
	return false;
}
