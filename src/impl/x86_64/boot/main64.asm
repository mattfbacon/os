global long_mode_start
extern kernel_main

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

	call kernel_main

	hlt