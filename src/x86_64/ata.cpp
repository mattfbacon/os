#include "io.h"
#include "ata.h"

bool identify_drive(bool secondary, uint16_t* data) {
	outb(0x1f6, secondary ? 0xb0 : 0xa0); // select drive
	outb(0x1f2, 0); // sectorcount = 0
	outb(0x1f3, 0); // LBAlo = 0
	outb(0x1f4, 0); // LBAmid = 0
	outb(0x1f5, 0); // LBAhi = 0
	outb(0x1f7, 0xec); // send IDENTIFY command
	while(inb(0x1f4) & (1 << 7)); // wait for BSY bit to clear
	if (inb(0x1f4) > 0 || inb(0x1f5) > 0) { // not ATA
		return false;
	}
	while (true) {
		uint8_t status = inb(0x1f7);
		if (status & (1 << 3)) { break; } // continue with the procedure
		if (status & (1 << 0)) { return false; } // error bit was set
	}
	for (size_t i = 0; i < 256; i++) {
		data[i] = inw(0x1f0);
	}
	return true;
}
