#pragma once

#include <stdint.h>

static inline void io_wait() {
	// Port 0x80 is used for 'checkpoints' during POST.
	// The Linux kernel seems to think it is free for use.
	asm volatile("outb %%al, $0x80" : : "a"(0));
}

static inline void outb(uint16_t const port, uint8_t const val) {
	asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t const port) {
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

static inline void outw(uint16_t const port, uint16_t const val) {
	asm volatile("outw %0, %1" : : "a"(val), "d"(port));
}

static inline uint16_t inw(uint16_t const port) {
	uint16_t ret;
	asm volatile("inw %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

// I'm using the d suffix instead of l because a doubleword is
// consistently 32 bits whereas the length of a long is dependent
// on number of bits. All extant 64-bit systems use 64-bit longs
// (the minimum according to the standard is 32 but they are 64
// nonetheless) so it makes no sense to use L to mean 32 bits.

static inline void outd(uint16_t const port, uint32_t const val) {
	asm volatile("outl %0, %1" : : "a"(val), "d"(port));
}

static inline uint32_t ind(uint16_t const port) {
	uint32_t ret;
	asm volatile("inl %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

static inline void dbg_halt() {
	asm volatile("xchg %bx, %bx\n\tcli\n\thlt");
}
