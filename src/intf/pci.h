#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PCI_CONFIG_ADDRESS 0xcf8
#define PCI_CONFIG_DATA 0xcfc

struct pci_device_type {
	uint8_t class;
	uint8_t subclass;
	uint8_t prog_if;
};

struct pci_device_index {
	uint8_t bus: 8;
	uint8_t slot: 4;
	uint8_t function: 3;
};

uint32_t pci_read_config(const uint8_t bus, const uint8_t slot, const uint8_t func, const uint8_t offset);
bool pci_find_device_by_type(struct pci_device_index*const out, const uint8_t class, const uint8_t subclass, const uint8_t prog_if);
