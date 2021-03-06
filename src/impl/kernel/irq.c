#include "print.h"
#include "io.h"

#define CTLR_PIC 0x20
#define FLWR_PIC 0xa0
#define CTLR_PIC_CMD (CTLR_PIC)
#define CTLR_PIC_DATA (CTLR_PIC + 1)
#define FLWR_PIC_CMD (FLWR_PIC)
#define FLWR_PIC_DATA (FLWR_PIC + 1)

#define PIC_EOI 0x20

#define ICW1_ICW4      0x01 /* ICW4 (not) needed */
#define ICW1_SINGLE    0x02 /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL     0x08 /* Level triggered (edge) mode */
#define ICW1_INIT      0x10 /* Initialization - required! */
 
#define ICW4_8086      0x01 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO      0x02 /* Auto (normal) EOI */
#define ICW4_BUF_FLRW  0x08 /* Buffered mode/follower */
#define ICW4_BUF_CTLR  0x0C /* Buffered mode/controller */
#define ICW4_SFNM      0x10 /* Special fully nested (not) */

void pic_sendEOI(const unsigned char irq) {
	// DEBUG: if (irq != 0) { print_ubyte_hex(irq); print_char(' '); }
	if (irq >= 8) outb(FLWR_PIC_CMD, PIC_EOI);
	outb(CTLR_PIC_CMD, PIC_EOI);
}

void pic_remap(const int offset1, const int offset2) {
	const unsigned char ctlr_mask = inb(CTLR_PIC_DATA), flwr_mask = inb(FLWR_PIC_DATA);
	outb(CTLR_PIC_CMD, ICW1_INIT | ICW1_ICW4); // start init sequence in cascade mode
	io_wait();
	outb(FLWR_PIC_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(CTLR_PIC_DATA, offset1); // controller offset
	io_wait();
	outb(FLWR_PIC_DATA, offset2); // follower offset
	io_wait();
	outb(CTLR_PIC_DATA, 2 << 1); // tell controller about follower at IRQ2
	io_wait();
	outb(FLWR_PIC_DATA, 2); // follower identity
	io_wait();

	outb(CTLR_PIC_DATA, ICW4_8086); // set mode
	io_wait();
	outb(FLWR_PIC_DATA, ICW4_8086);
	io_wait();

	outb(CTLR_PIC_DATA, ctlr_mask); // restore saved masks
	outb(FLWR_PIC_DATA, flwr_mask);
}

#define PIC_READ_IRR 0x0a /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b /* OCW3 irq service next CMD read */

static uint16_t __pic_get_irq_reg(const int ocw3) {
	outb(CTLR_PIC_CMD, ocw3);
	outb(FLWR_PIC_CMD, ocw3);
	return (inb(FLWR_PIC_CMD) << 8) | inb(CTLR_PIC_CMD);
}
 
uint16_t pic_get_irr() {
	return __pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr() {
	return __pic_get_irq_reg(PIC_READ_ISR);
}

void irq0_handler(void) {
	// Divide-by-zero
	pic_sendEOI(0);
}

void irq1_handler(void) {
	// Debug
	pic_sendEOI(1);
}

void irq2_handler(void) {
	// Non-maskable Interrupt
	pic_sendEOI(2);
}

void irq3_handler(void) {
	// Breakpoint
	pic_sendEOI(3);
}

void irq4_handler(void) {
	// Overflow
	pic_sendEOI(4);
}

void irq5_handler(void) {
	// Bound Range Exceeded
	pic_sendEOI(5);
}

void irq6_handler(void* opcode_addr) {
	// Invalid Opcode
	print_ensure_line();
	print_str("Got invalid opcode at ");
	print_ptr_hex(opcode_addr);
	print_char('\n');
	asm volatile ( "xchg %bx, %bx\n\tcli\n\thlt" );
	pic_sendEOI(6);
}

void irq7_handler(void) {
	// Device not Available / Spurious (on controller)
	if (pic_get_isr() & (1 << 7)) {
		// real interrupt
		pic_sendEOI(7);
	} else {
		// spurious, ignore
	}
}

void irq8_handler(void) {
	// Double Fault
	pic_sendEOI(8);
}

void irq9_handler(void) {
	// not used
	pic_sendEOI(9);
}

void irq10_handler(void) {
	// Invalid TSS
	pic_sendEOI(10);
}

void irq11_handler(void) {
	// Segment not Present
	pic_sendEOI(11);
}

void irq12_handler(void) {
	// Stack-Segment Fault
	pic_sendEOI(12);
}

void irq13_handler(void) {
	// General Protection Fault
	pic_sendEOI(13);
}

void irq14_handler(void) {
	// Page Fault
	pic_sendEOI(14);
}

void irq15_handler(void) {
	// Reserved / Spurious (on follower)
	if (pic_get_isr() & (1 << 15)) {
		// real interrupt
		pic_sendEOI(15);
	} else {
		// spurious, but still need to resolve controller
		pic_sendEOI(7);
	}
}

extern void irq0() asm("irq0"), irq1() asm("irq1"); // there are fifteen total but we only need the first two to calculate the rest

struct __attribute__((__packed__)) IDTDescr {
	uint16_t offset_1;  // offset bits 0..15
	uint16_t selector;  // a code segment selector in GDT or LDT
	uint8_t  ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
	uint8_t  type_attr; // type and attributes
	uint16_t offset_2;  // offset bits 16..31
	uint32_t offset_3;  // offset bits 32..63
	uint32_t zero;      // reserved
};

struct __attribute__((__packed__)) IDTPointer {
	uint16_t size; // size - 1
	uint64_t offset;
};

extern struct IDTDescr idt64 asm("idt64");
extern struct IDTPointer idt64_pointer asm("idt64.pointer");

#define CODE_SEGMENT_OFFSET 0x0008

void setup_idt() {
	const ptrdiff_t irq_size = irq1 - irq0;
	for (size_t i = 0; i < 256; i++) {
		const uint64_t this_irq_ptr = (uint64_t)irq0 + irq_size * (i % 16); // all the irqs are the same size so calculate it. max is 15 (implemented)
		(&idt64)[i] = (struct IDTDescr) {
			(uint16_t)this_irq_ptr,
			(uint16_t)CODE_SEGMENT_OFFSET,
			0, // no TSS yet
			0b10001110, // present, (2 bits) ring 0, interrupt/trap gate, (4 bits) 32-bit interrupt gate
			(uint16_t)(this_irq_ptr >> 16),
			(uint32_t)(this_irq_ptr >> 32),
			0
		};
	}
	idt64_pointer.size = (uint16_t)((uint64_t)&idt64_pointer - (uint64_t)&idt64 - 1);
	idt64_pointer.offset = (uint64_t)&idt64;
}
