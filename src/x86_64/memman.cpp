#include <cstddef>
#include <cstdint>

#include "bitops.hpp"
#include "memman.hpp"
#include "pid.hpp"

union page_table_entry {
	void* page_addr;
	struct __attribute__((packed)) {
		bool global: 1;
		bool page_size: 1;
		bool accessed: 1;
		bool no_cache: 1;
		bool write_through: 1;
		bool user_page: 1;
		bool writable: 1;
		bool present: 1;
	} attributes;
};

extern union page_table_entry user_page_table_l4[511][512];
extern union page_table_entry user_page_table_l3[511][512];
extern union page_table_entry user_page_table_l2[511][512];
extern union page_table_entry user_page_table_l1[511][512];

uint64_t linear_to_physical_addr(void const* const vptr_addr) {
	auto const addr = (uint64_t)vptr_addr;
	union {
		uint64_t* as_addr;
		uint64_t as_val;
	} table;
	table.as_addr = (uint64_t*)get_cr3();  // addr of l4 table
	table.as_val = table.as_addr[(addr >> 39) & ones(9)] & ~ones(12);  // get l3 base (l4 entry) given l4
	if (table.as_addr[(addr >> 30) & ones(9)] & bit_test(8)) {  // huge page 1 GiB
		return (table.as_val & ~ones(30)) + (addr & ones(30));  // get phys addr from phys page
	}
	// not 1 GiB huge page, so we can overwrite the table
	table.as_val = table.as_addr[(addr >> 30) & ones(9)] & ~ones(12);  // get l2 base (l3 entry) given l3
	if (table.as_addr[(addr >> 21) & ones(9)] & bit_test(8)) {  // huge page 2 MiB
		return (table.as_val & ~ones(21)) + (addr & ones(21));  // get phys addr from phys page
	}
	// not 2 MiB huge page, so we can overwrite the table
	table.as_val = table.as_addr[(addr >> 21) & ones(9)];  // get l1 base (l2 entry) given l2
	// there is no level 1 huge page so no huge page detection logic is required
	table.as_val = table.as_addr[(addr >> 12) & ones(9)] & ~ones(12);  // get phys page base (l1 entry) given l1

	// DEBUG: print_str("table addr: "); print_ptr_hex((void*)table.as_addr); print_newline();
	return table.as_val + (addr & ones(12));  // get phys addr from phys page
}

void setup_user_page_table(pid_t const process_id) {
	user_page_table_l4[process_id][0].page_addr = &(user_page_table_l3[process_id]);
	for (size_t i = 1; i < 512; i++) {
		user_page_table_l4[process_id][i].page_addr = 0;
	}
	user_page_table_l3[process_id][0].page_addr = &(user_page_table_l2[process_id]);
	for (size_t i = 1; i < 512; i++) {
		user_page_table_l3[process_id][i].page_addr = 0;
	}
	user_page_table_l2[process_id][0].page_addr = &(user_page_table_l1[process_id]);
	for (size_t i = 1; i < 512; i++) {
		user_page_table_l2[process_id][i].page_addr = 0;
	}
	// user_page_table_l1[process_id][0]
}
