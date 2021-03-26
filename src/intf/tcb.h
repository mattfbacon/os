struct __attribute__((packed)) tcb {
	struct tcb* next; // next task pointer (for scheduling; forms a circular linked list)
	uint64_t t_rsp; // stack pointer for the thread
	uint64_t t_cr3; // page dir for the thread

	uint8_t state; // task state bitfield: <structure TBD>
	uint8_t priority; // task priority: higher is more important
	uint8_t reserved_0;
	uint8_t owner_id; // owner process ID
};
