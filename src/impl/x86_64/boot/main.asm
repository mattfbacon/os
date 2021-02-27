global start
extern long_mode_start

RK equ 0x0c ; Red on blacK
WG equ 0x2f ; White on Green

section .text
bits 32
start:
	mov esp, stack_top

	call check_multiboot
	call check_cpuid
	call check_long_mode

	call setup_page_tables
	call enable_paging

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

setup_page_tables:
	mov eax, page_table_l3
	or eax, 0b11 ; present, writable
	mov [page_table_l4], eax

	mov eax, page_table_l2
	or eax, 0b11 ; present, writable
	mov [page_table_l3], eax

	mov ecx, 0 ; counter
.loop:
	mov eax, 2 * 1024 * 1024 ; 2 MiB
	mul ecx
	or eax, 0b1000_0011 ; huge page, present, writable
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
	dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; code segment: executable, code/data segment, present, 64-bit
.pointer:
	dw $ - gdt64 - 1
	dq gdt64
