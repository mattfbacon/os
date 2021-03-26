inline uint64_t get_cr3() {
	uint64_t cr3;
	asm ("mov %%cr3, %%rax\n\tmov %%rax, %0\n\t" : "=rm" (cr3) : : "%rax");
	return cr3;
}

uint64_t linear_to_physical_addr(const void*const vptr_addr);
// TODO: void setup_user_page_table(unsigned char process_id);
