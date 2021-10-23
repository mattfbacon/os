#pragma once

#include <cstddef>
#include <cstdint>

static constexpr uint16_t const PCI_CONFIG_ADDRESS = 0xcf8;
static constexpr uint16_t const PCI_CONFIG_DATA = 0xcfc;

struct pci_device_type {
	uint8_t class_;
	uint8_t subclass;
	uint8_t prog_if;
};

struct pci_device_index {
	uint8_t bus: 8;
	uint8_t slot: 4;
	uint8_t function: 3;
};

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
bool pci_find_device_by_type(pci_device_index* out, uint8_t class_, uint8_t subclass);
