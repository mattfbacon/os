global long_mode_start
extern kernel_main
extern setup_idt
extern pic_remap
extern setup_tss_in_gdt

extern gdt64.pointer
extern gdt64.tss
extern tss64

LK equ 0x07 ; Light gray on blacK

struc tss
	.reserved_1: resw 1
	.rsp0: resq 1
	.rsp1: resq 1
	.rsp2: resq 1
	.reserved_2: resq 1
	.ist1: resq 1
	.ist2: resq 1
	.ist3: resq 1
	.ist4: resq 1
	.ist5: resq 1
	.ist6: resq 1
	.ist7: resq 1
	.reserved_3: resq 1
	.reserved_4: resw 1
	.iopb_offset: resw 1
endstruc

section .text
bits 64
long_mode_start:
	; load null into data segment registers
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call setup_idt

	lidt [idt64.pointer]

	call pic_remap

	; this updates the TSS entry in the GDT as well as the GDT pointer
	call setup_tss_in_gdt
	lgdt [gdt64.pointer]

	; load task state segment
	mov ax, gdt64.tss
	or ax, 3 ; request user mode
	ltr ax

	sti

	call kernel_main

	; call jump_usermode

.halt:
	hlt
	jmp .halt

extern test_user_function
global jump_usermode
jump_usermode:
	mov rax, rsp
	; construct an iret stack frame (see https://wiki.osdev.org/Interrupt_Service_Routines#x86-64)
	push qword (4 * 8) | 3 ; stack selector
	push rax ; rsp in stack segment
	pushfq ; rflags
	push qword (3 * 8) | 3 ; code selector
	push test_user_function ; rip in code segment
	iretq

section .bss

global idt64
global idt64.pointer

idt64:
	resq 2 * 256
.pointer:
	resw 1
	resq 1

global dynamic_alloc_base
dynamic_alloc_base: ; the start of the dynamically allocated section of memory (really points to the start of the first header)
	resq 1
global num_allocd_sections
num_allocd_sections: ; the number of sections (32 MiB + 8 KiB header) that have been allocated.
; if there were more sections allocated then the `mapped_pages` structure would
; be duplicated to those positions
	resw 1
global first_free_page
first_free_page: ; pointer to the lowest free page
	resq 1
global mapped_pages
mapped_pages:
	resb 1024 * 8 ; each byte stores the process ID that owns the page (0 would be kernel)
alignb 1024 * 4
global user_page_table_l4
user_page_table_l4:
	resq 511 * 512
global user_page_table_l3
user_page_table_l3:
	resq 511 * 512
global user_page_table_l2
user_page_table_l2:
	resq 511 * 512 ; for each of 511 l3 entries, there are 512 in l2
global user_page_table_l1
user_page_table_l1:
	resq 511 * 512 ; due to the number of level 1 page tables, each process can be mapped a max of 2 mb unless huge pages are used
