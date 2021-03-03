#include "print.h"

#include <stdbool.h>
#include <limits.h>

static const size_t NUM_COLS = 80;
static const size_t NUM_ROWS = 25;

#define POS (col + NUM_COLS * row)

struct Char {
	uint8_t character;
	uint8_t color;
};

struct Char* buffer = (struct Char*)0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | (PRINT_COLOR_BLACK << 4);

void clear_row(size_t row) {
	struct Char empty = {
		.character = ' ', .color = color
	};

	for (size_t col = 0; col < NUM_COLS; col++) {
		buffer[POS] = empty;
	}
}

void print_clear() {
	for (size_t i = 0; i < NUM_ROWS; i++) {
		clear_row(i);
	}
}

void print_newline() {
	col = 0;

	if (row < NUM_ROWS - 1) {
		row++;
		return;
	}

	for (size_t cur_row = 1; cur_row < NUM_ROWS; cur_row++) {
		for (size_t cur_col = 0; cur_col < NUM_COLS; cur_col++) {
			buffer[cur_col + NUM_COLS * (cur_row - 1)] = buffer[cur_col + NUM_COLS * cur_row];
		}
	}

	clear_row(NUM_ROWS - 1);
}

void print_char(char character) {
	if (character == '\n') {
		print_newline();
		return;
	}
	if (col > NUM_COLS) {
		print_newline();
	}
	buffer[POS] = (struct Char) {
		.character = (uint8_t) character, .color = color
	};

	col++;
}

void print_str(const char* str) {
	for (; *str != '\0'; str += sizeof(char)) {
		print_char(*str);
	}
}

static const char DIGITS[16] __attribute__ ((nonstring)) = "0123456789abcdef"; // no null terminator
#define NEG_SYMBOL '-'
#define DECIMAL_SYMBOL '.'
#define HEX_MASK 0xf
#define HEX_BITS 4 // 4 bits per hex digit
#define BYTE_BITS 8 // 8 bits per byte
#define SHORT_BITS (sizeof(short) * BYTE_BITS)
#define INT_BITS (sizeof(int) * BYTE_BITS)
#define LONG_BITS (sizeof(long) * BYTE_BITS)
#define LLONG_BITS (sizeof(long long) * BYTE_BITS)
#define FLOAT_BITS (sizeof(float) * BYTE_BITS)
#define DOUBLE_BITS (sizeof(double) * BYTE_BITS)
#define LDOUBLE_BITS (sizeof(long double) * BYTE_BITS)
#define PTR_BITS 64 // number of bits in a pointer
#define EXTRA_HEX_BITS(size) ((size) - HEX_BITS) // number of bits after the top four
#define NUM_DIGITS (INT_BITS / HEX_BITS) // number of hex digits in an int

// INTEGRAL TYPES

#ifndef __GNUC__
#pragma region hex
#endif
// level 1: unsigned hex

void print_ushort_hex(unsigned short num) {
	for (size_t bits_left = SHORT_BITS; bits_left > 0; bits_left -= HEX_BITS, num <<= HEX_BITS) {
		print_char(DIGITS[(num & ((unsigned short)HEX_MASK << EXTRA_HEX_BITS(SHORT_BITS))) >> EXTRA_HEX_BITS(SHORT_BITS)]);
	}
}

void print_uint_hex(unsigned int num) {
	for (size_t bits_left = INT_BITS; bits_left > 0; bits_left -= HEX_BITS, num <<= HEX_BITS) {
		print_char(DIGITS[(num & ((unsigned int)HEX_MASK << EXTRA_HEX_BITS(INT_BITS))) >> EXTRA_HEX_BITS(INT_BITS)]);
	}
}

void print_ulong_hex(unsigned long num) {
	for (size_t bits_left = LONG_BITS; bits_left > 0; bits_left -= HEX_BITS, num <<= HEX_BITS) {
		print_char(DIGITS[(num & ((unsigned long)HEX_MASK << EXTRA_HEX_BITS(LONG_BITS))) >> EXTRA_HEX_BITS(LONG_BITS)]);
	}
}

void print_ullong_hex(unsigned long long num) {
	for (size_t bits_left = LLONG_BITS; bits_left > 0; bits_left -= HEX_BITS, num <<= HEX_BITS) {
		print_char(DIGITS[(num & ((unsigned long long)HEX_MASK << EXTRA_HEX_BITS(LLONG_BITS))) >> EXTRA_HEX_BITS(LLONG_BITS)]);
	}
}

void print_ubyte_hex(unsigned char num) {
	print_char(DIGITS[(num & ((unsigned char)HEX_MASK << HEX_BITS)) >> HEX_BITS]);
	print_char(DIGITS[num & HEX_MASK]);
}

void print_ptr_hex(void* ptr) {
	uint64_t num = (uint64_t)ptr;
	for (size_t bits_left = PTR_BITS; bits_left > 0; bits_left -= HEX_BITS, num <<= HEX_BITS) {
		print_char(DIGITS[(num & ((uint64_t)HEX_MASK << EXTRA_HEX_BITS(PTR_BITS))) >> EXTRA_HEX_BITS(PTR_BITS)]);
	}
}
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region bin
#endif
// level 1.5: unsigned bin

#define EXTRA_BIN_BITS(size) ((size) - 1)
#define BIN_MASK 0b1

void print_ushort_bin(unsigned short num) {
	for (size_t bits_left = SHORT_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((unsigned short)BIN_MASK << EXTRA_BIN_BITS(SHORT_BITS))) >> EXTRA_BIN_BITS(SHORT_BITS)]);
	}
}

void print_uint_bin(unsigned int num) {
	for (size_t bits_left = INT_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((unsigned int)BIN_MASK << EXTRA_BIN_BITS(INT_BITS))) >> EXTRA_BIN_BITS(INT_BITS)]);
	}
}

void print_ulong_bin(unsigned long num) {
	for (size_t bits_left = LONG_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((unsigned long)BIN_MASK << EXTRA_BIN_BITS(LONG_BITS))) >> EXTRA_BIN_BITS(LONG_BITS)]);
	}
}

void print_ullong_bin(unsigned long long num) {
	for (size_t bits_left = LLONG_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((unsigned long long)BIN_MASK << EXTRA_BIN_BITS(LLONG_BITS))) >> EXTRA_BIN_BITS(LLONG_BITS)]);
	}
}

void print_ubyte_bin(unsigned char num) {
	for (size_t bits_left = BYTE_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((unsigned char)BIN_MASK << EXTRA_BIN_BITS(BYTE_BITS))) >> EXTRA_BIN_BITS(BYTE_BITS)]);
	}
}

void print_ptr_bin(void* ptr) {
	uint64_t num = (uint64_t)ptr;
	for (size_t bits_left = PTR_BITS; bits_left > 0; bits_left--, num <<= 1) {
		print_char(DIGITS[(num & ((uint64_t)BIN_MASK << EXTRA_BIN_BITS(PTR_BITS))) >> EXTRA_BIN_BITS(PTR_BITS)]);
	}
}
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region dec
#endif
// levels 2 and 2.5: unsigned and signed dec
#define UCHAR_MAX_DIGITS 3
#define USHORT_MAX_DIGITS 5
#define UINT_MAX_DIGITS 10
#define ULONG_MAX_DIGITS 21
#define ULLONG_MAX_DIGITS 21 // in GCC ullong is same size as ulong

void print_ushort_dec(unsigned short num) {
	char buffer[USHORT_MAX_DIGITS] = { 0 };
	unsigned char idx = USHORT_MAX_DIGITS - 1;
	do {
		buffer[idx] = DIGITS[num % 10];
		idx--;
		num /= 10;
	} while (num != 0 && idx > 0);
	for (unsigned char i = idx + 1; i <= 4; i++) {
		print_char(buffer[i]);
	}
}
void print_short_dec(short num) {
	if (num < 0) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	print_ushort_dec((unsigned short)num);
}

void print_uint_dec(unsigned int num) {
	char buffer[UINT_MAX_DIGITS];
	unsigned char idx = UINT_MAX_DIGITS - 1;
	do {
		buffer[idx] = DIGITS[num % 10];
		idx--;
		num /= 10;
	} while (num != 0 && idx > 0);
	for (unsigned char i = idx + 1; i <= UINT_MAX_DIGITS - 1; i++) {
		print_char(buffer[i]);
	}
}
void print_int_dec(int num) {
	if (num < 0) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	print_uint_dec((unsigned int)num);
}

void print_ulong_dec(unsigned long num) {
	char buffer[ULONG_MAX_DIGITS];
	unsigned char idx = ULONG_MAX_DIGITS - 1;
	do {
		buffer[idx] = DIGITS[num % 10];
		idx--;
		num /= 10;
	} while (num != 0 && idx > 0);
	for (unsigned char i = idx + 1; i <= ULONG_MAX_DIGITS - 1; i++) {
		print_char(buffer[i]);
	}
}
void print_long_dec(long num) {
	if (num < 0) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	print_uint_dec((unsigned long)num);
}

void print_ullong_dec(unsigned long long num) {
	char buffer[ULLONG_MAX_DIGITS];
	unsigned char idx = ULLONG_MAX_DIGITS - 1;
	do {
		buffer[idx] = DIGITS[num % 10];
		idx--;
		num /= 10;
	} while (num != 0 && idx > 0);
	for (unsigned char i = idx + 1; i <= ULLONG_MAX_DIGITS - 1; i++) {
		print_char(buffer[i]);
	}
}
void print_llong_dec(long long num) {
	if (num < 0) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	print_ullong_dec((unsigned long long)num);
}

void print_ubyte_dec(unsigned char num) {
	char buffer[UCHAR_MAX_DIGITS];
	unsigned char idx = UCHAR_MAX_DIGITS - 1;
	do {
		buffer[idx] = DIGITS[num % 10];
		idx--;
		num /= 10;
	} while (num != 0 && idx > 0);
	for (unsigned char i = idx + 1; i <= UCHAR_MAX_DIGITS - 1; i++) {
		print_char(buffer[i]);
	}
}
void print_byte_dec(signed char num) {
	if (num < 0) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	print_ubyte_dec((unsigned char)num);
}
#ifndef __GNUC__
#pragma endregion
#endif

// OTHER NUMERIC TYPES

#ifndef __GNUC__
#pragma region others
#endif
void print_float(float num, size_t precision) {
	if (num < 0.0f) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	unsigned long long whole_part = (unsigned long long)num;
	print_ullong_dec(whole_part);
	if (precision == 0) return;
	num -= whole_part;
	print_char(DECIMAL_SYMBOL);
	num *= 10.0f;
	do {
		whole_part = (unsigned long long)num;
		num -= whole_part;
		print_char(DIGITS[(unsigned int)whole_part]);
		num *= 10.0f;
		precision--;
	} while (num != 0.0f && precision != 0);
}

void print_double(double num, size_t precision) {
	if (num < 0.0f) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	unsigned long long whole_part = (unsigned long long)num;
	print_ullong_dec(whole_part);
	if (precision == 0) return;
	num -= whole_part;
	print_char(DECIMAL_SYMBOL);
	num *= 10.0f;
	do {
		whole_part = (unsigned long long)num;
		num -= whole_part;
		print_char(DIGITS[(unsigned int)whole_part]);
		num *= 10.0f;
		precision--;
	} while (num != 0.0f && precision != 0);
}

void print_ldouble(long double num, size_t precision) {
	if (num < 0.0f) {
		print_char(NEG_SYMBOL);
		num = -num;
	}
	unsigned long long whole_part = (unsigned long long)num;
	print_ullong_dec(whole_part);
	if (precision == 0) return;
	num -= whole_part;
	print_char(DECIMAL_SYMBOL);
	num *= 10.0f;
	do {
		whole_part = (unsigned long long)num;
		num -= whole_part;
		print_char(DIGITS[(unsigned int)whole_part]);
		num *= 10.0f;
		precision--;
	} while (num != 0.0f && precision != 0);
}
#ifndef __GNUC__
#pragma endregion
#endif

void print_set_color(uint8_t fore, uint8_t back) {
	color = fore | (back << 4);
}

void print_set_color_raw(uint8_t raw_color) {
	color = raw_color;
}

void print_ensure_line() {
	if (col != 0) print_newline();
}
