struct __attribute__((packed)) tcb {
	struct tcb* next; // next task pointer (for scheduling)
	uint64_t t_rsp; // stack pointer for the thread
	uint64_t t_cr3; // paging control for the thread
	uint64_t k_rsp; // kernel stack pointer for this thread

	uint8_t state; // task state bitfield: <structure TBD>
	uint8_t priority; // task priority: higher is more important
	uint16_t reserved_0;
	uint16_t owner_id; // owner process ID
	uint16_t task_id; // task ID under process
};
