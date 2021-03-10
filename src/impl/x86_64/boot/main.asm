global start
extern long_mode_start

RK equ 0x0c ; Red on blacK

section .text
bits 32
start:
	mov esp, stack_top

	call check_multiboot
	call check_cpuid
	call check_long_mode
	call check_apic
	call check_fpu

	; setup the FPU
	mov edx, cr4
	or edx, (1 << 9) | (1 << 10) ; set SSE support (allow ops) and XF Exception (want to have this to be able to handle SSE exceptions properly)
	mov cr4, edx

	call setup_page_tables
	call enable_paging

	cli ; likely redundant but is a best practice
	lgdt [gdt64.pointer]

	jmp gdt64.code_segment:long_mode_start

	hlt

check_multiboot:
	; magic number from multiboot
	cmp eax, 0x36d76289
	jne .no_multiboot
	ret
.no_multiboot:
	mov al, 'M'
	jmp error

check_cpuid:
	; copy cpu flags to eax
	pushfd
	pop eax
	; keep a copy
	mov ecx, eax
	; flip the id bit
	xor eax, 1 << 21
	; set the cpu flags with our modified version
	push eax
	popfd
	; copy (hopefully) modified flags to eax
	pushfd
	pop eax
	; reset flags after modifying
	push ecx
	popfd
	; compare changed to original
	cmp eax, ecx
	je .no_cpuid
	ret
.no_cpuid:
	mov al, 'C'
	jmp error

check_long_mode:
	; cpu will increment eax by some amount if it supports long mode
	mov eax, 0x80_00_00_00
	cpuid
	cmp eax, 0x80_00_00_01
	; if less, then no long mode
	jb .no_long_mode

	; another test for long mode
	mov eax, 0x80_00_00_01
	cpuid
	test edx, 1 << 29 ; LM bit
	; if LM bit is not set, no long mode
	jz .no_long_mode

	ret
.no_long_mode:
	mov al, 'L'
	jmp error

check_apic:
	mov eax, 0x01
	cpuid
	test edx, 1 << 9
	jz .no_apic

	ret
.no_apic:
	mov al, 'A'
	jmp error

check_fpu:
	mov edx, cr0
	and edx, (-1) - ((1 << 4) + (1 << 2)) ; clear TS (task switch; causes exceptions for ops) and EM (allow FPU ops)
	mov cr0, edx
	fninit ; load defaults to FPU
	fnstsw [.testword] ; try to save status
	cmp word [.testword], 0 ; check if status was saved
	jne .no_fpu

	ret
.no_fpu:
	mov al, 'F'
	jmp error
.testword: dw 0x55aa ; garbage

setup_page_tables:
	mov eax, page_table_l3
	or eax, 11b ; present, writable
	mov [page_table_l4], eax

	mov eax, page_table_l2
	or eax, 11b ; present, writable
	mov [page_table_l3], eax

	mov ecx, 0 ; counter
.loop:
	mov eax, 2 * 1024 * 1024 ; 2 MiB
	mul ecx
	or eax, 1000_0011b ; huge page, present, writable
	mov [page_table_l2 + ecx * 8], eax

	inc ecx
	cmp ecx, 512 ; 512 entries in the level 2 table
	jne .loop

	ret

enable_paging:
	; pass page table location to CPU
	mov eax, page_table_l4
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5 ; PAE flag
	mov cr4, eax

	; enable long mode
	mov ecx, 0xc0_00_00_80 ; magic value for model-specific register
	rdmsr ; read model-specific register
	or eax, 1 << 8 ; enable long-mode flag
	wrmsr ; write MSR

	; enable paging
	mov eax, cr0
	or eax, 1 << 31 ; paging flag
	mov cr0, eax

	ret

error:
	; print "ERR: X" where X is error code in al
	mov byte [0xb8000], 'E'
	mov byte [0xb8001], RK
	mov byte [0xb8002], 'R'
	mov byte [0xb8003], RK
	mov byte [0xb8004], 'R'
	mov byte [0xb8005], RK
	mov byte [0xb8006], ':'
	mov byte [0xb8007], RK
	mov byte [0xb8008], ' '
	mov byte [0xb8009], RK
	mov byte [0xb800a], al ; the error code
	mov byte [0xb800b], RK
	hlt

section .bss
align 1024 * 4
page_table_l4:
	resb 1024 * 4
page_table_l3:
	resb 1024 * 4
page_table_l2:
	resb 1024 * 4
stack_bottom:
	resb 1024 * 16 ; 16kb
stack_top:

section .rodata

gdt64:
	dq 0 ; zero entry
.code_segment: equ $ - gdt64
	dw 0xffff ; limit bits 0:15
	dw 0x00_00 ; base bits 0:15
	db 0x00 ; base bits 16:23
	db 1001_1010b ; access byte: present, (2 bits) ring 0, code/data segment, executable, code out of ring 0 cannot call this code, readable, not accessed
	db 1010_1111b ; (0) 4kib blocks for limit, (1:3) flags (010 = 64-bit), (4:7) limit bits 16:19
	db 0x00 ; base bits 24:31
.data_segment: equ $ - gdt64
	dw 0xffff
	dw 0x00_00
	db 0x00
	db 1001_0010b ; not executable, writable
	db 1010_1111b
	db 0x00
.ring3_code: equ $ - gdt64
	dw 0xffff
	dw 0x0000
	db 0x00
	db 1111_1010b ; (0) page gran, (1-2) ring 3, (5) conforming bit is irrelevant
	db 1010_1111b
	db 0x00
.ring3_data: equ $ - gdt64
	dw 0xffff
	dw 0x0000
	db 0x00
	db 1111_0010b ; same but bit 4 is clear for a data segment
	db 1010_1111b
	db 0x00
.pointer: ; the GDT descriptor
	dw $ - gdt64 - 1 ; size - 1
	dq gdt64 ; offset
