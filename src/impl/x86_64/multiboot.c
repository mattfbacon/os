#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"
#include "print.h"

const struct multiboot_tag* find_multiboot_tag_by_type(const uint32_t type, const struct multiboot_tag* start) {
	union {
		const struct multiboot_tag* as_addr;
		uint64_t as_val;
	} ptr;
	ptr.as_addr = start != NULL ? start : (struct multiboot_tag*)(multiboot_data + 1);
	while (ptr.as_val < ((uint64_t)multiboot_data + multiboot_data->total_size)) {
		if (ptr.as_addr->type == type) {
			return ptr.as_addr; // found the tag
		}
		ptr.as_val += ptr.as_addr->size;
		if (ptr.as_val & 0b111) { // not 8-byte aligned
			ptr.as_val &= ~0b111; // align to 8 bytes...
			ptr.as_val += 8; // ... rounding up because this is padding
		}
	}
	return NULL; // no tag of that type was found (loop exited due to reaching end of multiboot data)
}
