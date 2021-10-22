#include <stddef.h>
#include <stdint.h>

struct __attribute__((packed)) gdt_tss_entry {
	// see https://xem.github.io/minix86/manual/intel-x86-and-64-manual-vol3/o_fe12b1e2a880e0ce-245.html
	uint16_t limit_low;
	uint32_t base_low: 24;
	uint8_t access;
	uint8_t limit_high: 4;
	uint8_t attrs: 4;
	uint64_t base_high: 40;  // 32 bits added in 64-bit mode
	// this section added to TSS descriptors in 64-bit mode
	uint8_t reserved_0;
	uint8_t zero: 4;
	uint32_t reserved_1: 20;
};

struct __attribute__((packed)) gdt_pointer {
	uint16_t limit;  // size - 1
	struct gdt_entry* base;
};

extern void gdt64(void) asm("gdt64");
extern struct gdt_tss_entry gdt64_tss asm("gdt64.tss_abs");
extern struct gdt_pointer gdt64_pointer asm("gdt64.pointer");

extern void tss64(void) asm("tss64") __attribute__((error("Not a function")));

extern "C" void setup_tss_in_gdt() {
	gdt64_tss.base_low = (uint64_t)&tss64;
	gdt64_tss.base_high = (uint64_t)&tss64 >> 24;

	gdt64_pointer.limit = (uint64_t)&gdt64_pointer - (uint64_t)&gdt64 - 1;
}
