%include "tcb.inc"

extern tss64.rsp0

global switch_to_task

section .text
;; void switch_to_task(struct TCB* next_thread);
; must actually call the task after running this
switch_to_task:
	; exchange new task with old in memory
	mov rsi, rdi ; save new TCB*
	; writes new TCB* to memory and old TCB* to rdi
	xchg [current_task_TCB], rdi

	; save prev task
	mov [rdi + TCB.T_RSP], rsp ; save current stack pointer

	; load next task (new TCB* is in rsi)
	mov rsp, [rsi + TCB.T_RSP] ; load new task's stack pointer
	mov rax, [rsi + TCB.T_CR3] ; load new task's page dir (will be mov'd into cr3)
	mov rcx, [rsi + TCB.K_RSP] ; load new task's kernel stack...
	mov [tss64.rsp0], rcx ; ...and save it in the TSS

; handle page dir update
	mov rcx, cr3 ; old task's page dir
	cmp rax, rcx ; compare new and old page dirs
	je .skip_page_dir_change ; if they are the same, avoid updating cr3
	mov cr3, rax
.skip_page_dir_change:

	ret

;; void set_kernel_stack(void* rsp);
set_kernel_stack:
	; rdi is first arg
	mov [tss64.rsp0], rdi
	ret

section .bss
current_task_TCB resq 1 ; pointer to TCB for current task
