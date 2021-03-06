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

extern irq_handler
global irq_base
global irq_limit

irq_base:
	%assign int_num 0
	; 0 – 255
	%rep 256
		pushaq
		cld
		mov rdi, int_num
		mov rsi, [rsp + 72] ; get instruction pointer from the stack (72 = 8 bytes * 9 registers)
		call irq_handler
		popaq
		iretq

		%assign int_num int_num + 1
	%endrep
irq_limit:
