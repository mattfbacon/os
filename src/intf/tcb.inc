struc TCB ; thread control block
	.NEXT:  resq 1 ; next task (pointer to another TCB)
	.T_RSP: resq 1 ; stack pointer for this thread
	.T_CR3: resq 1 ; paging control register
	.K_RSP: resq 1 ; stack pointer for kernel

	.STATE: resb 1 ; task state bitfield: <structure TBD>
	.PRIO:  resb 1 ; task priority (higher is more important)
	.RESVD: resw 1 ; reserved
	.OWNER: resw 1 ; owner process ID
	.T_ID:  resw 1 ; task ID under process
endstruc
