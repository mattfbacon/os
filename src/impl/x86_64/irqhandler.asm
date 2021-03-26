section .text

%macro pushaq_not_rdx 0
	; does not push rdx!
	push rax
	push rcx
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
	pop rcx
	pop rax
	pop rdx
%endmacro

%define int_has_error_code(irq) ((irq) == 8 || ((irq) >= 10 && (irq) < 15) || (irq) == 17 || (irq) == 30)

extern irq_handler
global irq_base
global irq_limit

irq_base:
	%assign int_num 0
	; 0 – 255
	%rep 256
	int_%[int_num]:
		%if int_has_error_code(int_num)
			; the error code is a dword, so if we advance the stack pointer by 4
			; bytes (dword) we can pretend it's a qword. Then swap with rdx and we
			; end up with rdx on the stack and the error code (left-shifted by 32
			; bits aka a dword) in rdx.
			sub rsp, 4
			xchg rdx, qword [rsp]
			; undo the left shift
			shr rdx, 32
			; now rdx holds the error code
		%else
			push rdx
		%endif

		pushaq_not_rdx
		cld ; restored because rflags is pushed for interrupts
		mov rdi, int_num
		mov rsi, qword [rsp + 72] ; get saved rip from the stack (72 = 8 bytes * 9 registers pushed)
		; error code is in rdx (will be garbage if there is no error code)
		call irq_handler
		popaq
		iretq

		%assign int_num int_num + 1
	%endrep
irq_limit:

section .rodata
global int_table
int_table:
%assign int_num 0
%rep 256
	dq int_%[int_num]
	%assign int_num int_num + 1
%endrep
