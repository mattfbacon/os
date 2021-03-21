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

halt:
	hlt
	jmp halt

section .bss

global idt64
global idt64.pointer

idt64:
	resq 2 * 256
.pointer:
	resw 1
	resq 1
