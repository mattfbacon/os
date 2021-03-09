global long_mode_start
extern kernel_main
extern setup_idt
extern pic_remap

LK equ 0x07 ; Light gray on blacK

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
