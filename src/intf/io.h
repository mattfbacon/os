#pragma once

#include<stdint.h>

static inline void io_wait() {
	// Port 0x80 is used for 'checkpoints' during POST.
	// The Linux kernel seems to think it is free for use.
	asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

static inline void outb(const uint16_t port, const uint8_t val) {
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(const uint16_t port) {
	uint8_t ret;
	asm volatile ( "inb %1, %0"
		: "=a"(ret)
		: "Nd"(port) );
	return ret;
}
