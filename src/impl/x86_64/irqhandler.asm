global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler

%macro pushaq 0
	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
%endmacro

%macro popaq 0
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax
%endmacro

irq0:
	pushaq
	cld
	call irq0_handler
	popaq
	iretq
 
irq1:
	pushaq
	cld
	call irq1_handler
	popaq
	iretq
 
irq2:
	pushaq
	cld
	call irq2_handler
	popaq
	iretq
 
irq3:
	pushaq
	cld
	call irq3_handler
	popaq
	iretq
 
irq4:
	pushaq
	cld
	call irq4_handler
	popaq
	iretq
 
irq5:
	pushaq
	cld
	call irq5_handler
	popaq
	iretq
 
irq6:
	pushaq
	cld
	mov rdi, [esp + 72] ; pass address of offending instruction to handler (72 to avoid the 9 qword pushed registers)
	call irq6_handler
	popaq
	iretq
 
irq7:
	pushaq
	cld
	call irq7_handler
	popaq
	iretq
 
irq8:
	pushaq
	cld
	call irq8_handler
	popaq
	iretq
 
irq9:
	pushaq
	cld
	call irq9_handler
	popaq
	iretq
 
irq10:
	pushaq
	cld
	call irq10_handler
	popaq
	iretq
 
irq11:
	pushaq
	cld
	call irq11_handler
	popaq
	iretq
 
irq12:
	pushaq
	cld
	call irq12_handler
	popaq
	iretq
 
irq13:
	pushaq
	cld
	call irq13_handler
	popaq
	iretq
 
irq14:
	pushaq
	cld
	call irq14_handler
	popaq
	iretq
 
irq15:
	pushaq
	cld
	call irq15_handler
	popaq
	iretq
