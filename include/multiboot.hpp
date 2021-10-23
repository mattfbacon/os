#pragma once
#include <cstdint>

#define MULTIBOOT_BOOT_CMDLINE_TYPE 1
#define MULTIBOOT_MMAP_TYPE 6
#define MULTIBOOT_EFI_NOT_TERMINATED_TYPE 18

extern const struct multiboot_data* multiboot_data asm("multiboot_data_ptr");

struct __attribute__((packed)) multiboot_data {
	uint32_t total_size;
	uint32_t _reserved;
};

struct __attribute__((packed)) multiboot_tag {
	uint32_t type;
	uint32_t size;
};

struct __attribute__((packed)) multiboot_mmap_entry {
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
	uint32_t reserved;
};

struct __attribute__((packed)) multiboot_tag_mmap {
	uint32_t _type;  // = 6
	uint32_t size;
	uint32_t entry_size;
	uint32_t entry_version;
	struct multiboot_mmap_entry head;  // not a char
};

struct __attribute__((packed)) multiboot_tag_boot_cmdline {
	uint32_t _type;  // = 1
	uint32_t size;
	char cmdline[];
};

multiboot_tag const* find_multiboot_tag_by_type(uint32_t type, multiboot_tag const* start);

static inline const multiboot_tag_mmap* find_multiboot_mmap() {
	return (multiboot_tag_mmap*)find_multiboot_tag_by_type(MULTIBOOT_MMAP_TYPE, nullptr);
}

static inline bool were_multiboot_services_terminated() {
	return find_multiboot_tag_by_type(MULTIBOOT_EFI_NOT_TERMINATED_TYPE, nullptr) != nullptr;
}

static inline char const* get_boot_cmdline() {
	return ((multiboot_tag_boot_cmdline*)find_multiboot_tag_by_type(MULTIBOOT_BOOT_CMDLINE_TYPE, nullptr))->cmdline;
}
