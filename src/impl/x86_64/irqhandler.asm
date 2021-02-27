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

%macro push64 0
; the registers that need to be saved according to GCC calling convention (see https://stackoverflow.com/a/14486309/4945014)
push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
push rsi
push rdi
%endmacro

%macro pop64 0
pop rdi
pop rsi
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
%endmacro

irq0:
	push64
	call irq0_handler
	pop64
	iret
 
irq1:
	push64
	call irq1_handler
	pop64
	iret
 
irq2:
	push64
	call irq2_handler
	pop64
	iret
 
irq3:
	push64
	call irq3_handler
	pop64
	iret
 
irq4:
	push64
	call irq4_handler
	pop64
	iret
 
irq5:
	push64
	call irq5_handler
	pop64
	iret
 
irq6:
	push64
	call irq6_handler
	pop64
	iret
 
irq7:
	push64
	call irq7_handler
	pop64
	iret
 
irq8:
	push64
	call irq8_handler
	pop64
	iret
 
irq9:
	push64
	call irq9_handler
	pop64
	iret
 
irq10:
	push64
	call irq10_handler
	pop64
	iret
 
irq11:
	push64
	call irq11_handler
	pop64
	iret
 
irq12:
	push64
	call irq12_handler
	pop64
	iret
 
irq13:
	push64
	call irq13_handler
	pop64
	iret
 
irq14:
	push64
	call irq14_handler
	pop64
	iret
 
irq15:
	push64
	call irq15_handler
	pop64
	iret
