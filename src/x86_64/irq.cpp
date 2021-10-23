#include "io.hpp"
#include "print.hpp"

using irq_port_t = uint16_t;
using irq_id_t = uint8_t;

static constexpr irq_port_t const KBD_DATA = 0x60;

static constexpr irq_id_t const CTLR_IRQ_OFFSET = 0x20;
static constexpr irq_id_t const FLWR_IRQ_OFFSET = 0x28;
static constexpr size_t const PIC_NUM_INTERRUPTS = 8;
constexpr irq_id_t XLATE_TO_CTLR_IRQ(irq_id_t const flwr_irq) {
	return flwr_irq - (FLWR_IRQ_OFFSET - CTLR_IRQ_OFFSET);
}

static constexpr irq_port_t const CTLR_PIC = 0x20;
static constexpr irq_port_t const FLWR_PIC = 0xa0;
static constexpr irq_port_t const CTLR_PIC_CMD = CTLR_PIC;
static constexpr irq_port_t const CTLR_PIC_DATA = CTLR_PIC + 1;
static constexpr irq_port_t const FLWR_PIC_CMD = FLWR_PIC;
static constexpr irq_port_t const FLWR_PIC_DATA = FLWR_PIC + 1;

static constexpr uint8_t const PIC_EOI = 0x20;

static constexpr uint8_t const ICW1_ICW4 = 0x01; /* ICW4 (not) needed */
static constexpr uint8_t const ICW1_SINGLE = 0x02; /* Single (cascade) mode */
static constexpr uint8_t const ICW1_INTERVAL4 = 0x04; /* Call address interval 4 (8) */
static constexpr uint8_t const ICW1_LEVEL = 0x08; /* Level triggered (edge) mode */
static constexpr uint8_t const ICW1_INIT = 0x10; /* Initialization - required! */

static constexpr uint8_t const ICW4_8086 = 0x01; /* 8086/88 (MCS-80/85) mode */
static constexpr uint8_t const ICW4_AUTO = 0x02; /* Auto (normal) EOI */
static constexpr uint8_t const ICW4_BUF_FLRW = 0x08; /* Buffered mode/follower */
static constexpr uint8_t const ICW4_BUF_CTLR = 0x0C; /* Buffered mode/controller */
static constexpr uint8_t const ICW4_SFNM = 0x10; /* Special fully nested (not) */

void pic_sendEOI(uint8_t const irq) {
	// DEBUG: if (irq != 0) { print_ubyte_hex(irq); print_char(' '); }
	if (irq >= CTLR_IRQ_OFFSET && irq < FLWR_IRQ_OFFSET + PIC_NUM_INTERRUPTS) {
		if (irq >= FLWR_IRQ_OFFSET) outb(FLWR_PIC_CMD, PIC_EOI);
		outb(CTLR_PIC_CMD, PIC_EOI);
	}
}

extern "C" void pic_remap() {
	auto ctlr_mask = inb(CTLR_PIC_DATA), flwr_mask = inb(FLWR_PIC_DATA);
	outb(CTLR_PIC_CMD, ICW1_INIT | ICW1_ICW4);  // start init sequence in cascade mode
	io_wait();
	outb(FLWR_PIC_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(CTLR_PIC_DATA, CTLR_IRQ_OFFSET);  // controller offset
	io_wait();
	outb(FLWR_PIC_DATA, FLWR_IRQ_OFFSET);  // follower offset
	io_wait();
	outb(CTLR_PIC_DATA, 2 << 1);  // tell controller about follower at IRQ2
	io_wait();
	outb(FLWR_PIC_DATA, 2);  // follower identity
	io_wait();

	outb(CTLR_PIC_DATA, ICW4_8086);  // set mode
	io_wait();
	outb(FLWR_PIC_DATA, ICW4_8086);
	io_wait();

	outb(CTLR_PIC_DATA, ctlr_mask);  // restore saved masks
	outb(FLWR_PIC_DATA, flwr_mask);
}

static constexpr uint8_t const PIC_READ_IRR = 0x0a; /* OCW3 irq ready next CMD read */
static constexpr uint8_t const PIC_READ_ISR = 0x0b; /* OCW3 irq service next CMD read */

static uint16_t __pic_get_irq_reg(uint8_t const ocw3) {
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

extern "C" void irq_handler(irq_id_t const irq, void const* const rip, uint32_t const err_code) {
	switch (irq) {
		case 0x6:
			print_ensure_line();
			print_set_color(PRINT_COLOR_BLACK, PRINT_COLOR_RED);
			print_str("Got invalid opcode at ");
			print_ptr_hex(rip);
			print_newline();
			dbg_halt();
			return;
		case 0xd:
			print_ensure_line();
			print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLUE);
			print_str("#GP!! ");
			print_ptr_hex(rip);
			print_newline();
			print_ubyte_hex((unsigned char)err_code);
			print_newline();
			dbg_halt();
			return;
		case 0xe:
			print_ensure_line();
			print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BROWN);
			print_str("#PF!! ");
			print_ptr_hex(rip);
			print_newline();
			print_ubyte_bin((unsigned char)err_code);
			print_newline();
			dbg_halt();
			return;
		case 0x20:  // timer interrupt, nothing to do for now, ignore
			break;
		case 0x21:  // keyboard
			print_set_color(PRINT_COLOR_LIGHT_GRAY, PRINT_COLOR_BLACK);
			print_ubyte_hex(inb(KBD_DATA));
			print_char(' ');
			break;
		case CTLR_IRQ_OFFSET + PIC_NUM_INTERRUPTS - 1:  // controller possible spurious
			if (pic_get_isr() & (1 << 7)) {
				// real interrupt
				pic_sendEOI(irq);
			}  // else spurious, ignore
			return;
		case FLWR_IRQ_OFFSET + PIC_NUM_INTERRUPTS - 1:  // follower possible spurious
			if (pic_get_isr() & (1 << 15)) {
				// real interrupt
				pic_sendEOI(irq);
			} else {
				// spurious, but still need to resolve controller
				pic_sendEOI(XLATE_TO_CTLR_IRQ(irq));
			}
			return;
		case 0x2e:  // ? get this during ATA PIO, what is it for??
			break;
		case 0x80:
			print_str("INT");
			return;
		default:
			print_char('I');
			print_ubyte_hex(irq);
			print_char(' ');
			break;
	}
	pic_sendEOI(irq);
}

// no code is actually at irq_limit, and irq_base should not be called, but
// it's convenient to have them as functions because GCC will make them
// pointers automatically when they're used in an expression (no & required).
extern void irq_base(void) asm("irq_base") __attribute__((error("Not a function")));
extern void irq_limit(void) asm("irq_limit") __attribute__((error("Not a function")));  // 256 interrupts

struct __attribute__((packed)) IDTDescr {
	uint16_t offset_1;  // offset bits 0..15
	uint16_t selector;  // a code segment selector in GDT or LDT
	uint8_t ist;  // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
	uint8_t type_attr;  // type and attributes
	uint16_t offset_2;  // offset bits 16..31
	uint32_t offset_3;  // offset bits 32..63
	uint32_t zero;  // reserved
};

struct __attribute__((packed)) IDTPointer {
	uint16_t size;  // size - 1
	uint64_t offset;
};

// would do the same function trick here but we use array indexing which is
// syntactic sugar that I don't want to give up
extern struct IDTDescr idt64[256] asm("idt64");
extern struct IDTPointer idt64_pointer asm("idt64.pointer");
extern uint64_t int_table[256] asm("int_table");

#define CODE_SEGMENT_OFFSET 0x0008

extern "C" void setup_idt() {
	for (size_t i = 0; i < 256; i++) {
		auto const this_irq_ptr = int_table[i];  // all the irqs are the same size (just wrappers around C functions), so calculate the address.
		// clang-format off
		idt64[i] = {
			(uint16_t)this_irq_ptr,
			(uint16_t)CODE_SEGMENT_OFFSET,
			1,  // use interrupt stack #1
			0b10001110,  // present, (2 bits) ring 0, interrupt/trap gate, (4 bits) 32-bit interrupt gate
			(uint16_t)(this_irq_ptr >> 16),
			(uint32_t)(this_irq_ptr >> 32),
			0
		};
		// clang-format on
	}
	idt64_pointer.size = (uint16_t)((uint64_t)&idt64_pointer - (uint64_t)idt64 - 1);
	idt64_pointer.offset = (uint64_t)idt64;
}
