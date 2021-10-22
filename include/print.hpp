#pragma once

#include <stddef.h>
#include <stdint.h>

enum PrintColor : uint8_t {
	PRINT_COLOR_BLACK = 0,
	PRINT_COLOR_BLUE = 1,
	PRINT_COLOR_GREEN = 2,
	PRINT_COLOR_CYAN = 3,
	PRINT_COLOR_RED = 4,
	PRINT_COLOR_MAGENTA = 5,
	PRINT_COLOR_BROWN = 6,
	PRINT_COLOR_LIGHT_GRAY = 7,
	PRINT_COLOR_DARK_GRAY = 8,
	PRINT_COLOR_LIGHT_BLUE = 9,
	PRINT_COLOR_LIGHT_GREEN = 10,
	PRINT_COLOR_LIGHT_CYAN = 11,
	PRINT_COLOR_LIGHT_RED = 12,
	PRINT_COLOR_PINK = 13,
	PRINT_COLOR_YELLOW = 14,
	PRINT_COLOR_WHITE = 15,
};

#ifdef __cplusplus
static constexpr size_t const NUM_ROWS = 25;
static constexpr size_t const NUM_COLS = 80;
static constexpr size_t const TAB_SIZE = 8;
#else
#define NUM_ROWS 25
#define NUM_COLS 80
#define TAB_SIZE 8
#endif

#ifdef __cplusplus
#include "std/array.hpp"

struct __attribute__((packed)) Color {
	uint8_t data;
protected:
	static constexpr size_t const BACKGROUND_SHIFT = 4;
public:
	Color& set_foreground(PrintColor const value) {
		data &= ~(uint8_t)0b1111;
		data |= value;
		return *this;
	}
	Color& set_background(PrintColor const value) {
		data &= ~((uint8_t)0b1111 << BACKGROUND_SHIFT);
		data |= value << BACKGROUND_SHIFT;
		return *this;
	}
	explicit Color(PrintColor const foreground = PRINT_COLOR_WHITE, PrintColor const background = PRINT_COLOR_BLACK) {
		set_foreground(foreground);
		set_background(background);
	}
	explicit Color(uint8_t raw) : data(raw) {}
	operator uint8_t() {
		return data;
	}
};

struct __attribute__((packed)) ScreenChar {
	char character;
	Color color;
	void operator=(ScreenChar const& other) volatile {
		character = other.character;
		color.data = other.color.data;
	}
};

template <size_t Rows = NUM_ROWS, size_t Cols = NUM_COLS, size_t TabSize = TAB_SIZE>
struct Screen {
	struct {
		std::array<ScreenChar, Cols>& operator[](size_t idx) {
			return ((std::array<ScreenChar, Cols>*)0xb8000)[idx];
		}
	} location;
	struct {
		size_t row = 0;
		size_t col = 0;
	} position;
	Color color;
public:
	Screen() {}
	Screen& clear_row(size_t const row_to_clear) {
		for (size_t col = 0; col < Cols; col++) {
			location[row_to_clear][col] = { ' ', this->color };
		}
		if (position.row == row_to_clear) {
			position.col = 0;
		}
		return *this;
	}
	Screen& clear() {
		for (size_t row = 0; row < Rows; row++) {
			for (size_t col = 0; col < Cols; col++) {
				location[row][col] = { ' ', this->color };
			}
		}
		position = { 0, 0 };
		return *this;
	}
	Screen& print_newline() {
		position.col = 0;
		// last row requires shifting
		if (position.row == Rows - 1) {
			for (size_t row = 1; row < Rows; row++) {
				location[row - 1] = location[row];
			}
		} else {
			position.row++;
		}
		return *this;
	}
protected:
	void print_regular_char(char const character, Color const color) {
		if (position.col >= Cols) {
			print_newline();
		}
		location[position.row][position.col] = (ScreenChar){ character, color };
		position.col++;
	}
public:
	Screen& print_char(char const character, Color const color) {
		switch (character) {
			case '\n':  // newline
				print_newline();
				break;
			case '\r':  // carriage return
				position.col = 0;
				break;
			case '\v': {  // vertical tab
				size_t const old_col = position.col;
				print_newline();
				position.col = old_col;
				break;
			}
			case '\f':  // form feed: ensure empty line
				if (position.col != 0) {
					print_newline();
				}
				break;
			case '\t':  // tab
				for (size_t i = 0; i < TabSize; i++) {
					print_char(' ');
				}
				break;
			default:
				print_regular_char(character, color);
				break;
		}
		return *this;
	}
	Screen& print_char(char const character) {
		return print_char(character, this->color);
	}
	Screen& print_str(char const* str, Color const color) {
		for (; *str != '\0'; str++) {
			print_char(*str, color);
		}
		return *this;
	}
	Screen& print_str(char const* str) {
		return print_str(str, this->color);
	}
};

extern template class Screen<NUM_ROWS, NUM_COLS, TAB_SIZE>;
extern Screen<NUM_ROWS, NUM_COLS, TAB_SIZE> g_screen;

#endif

#ifdef __cplusplus
extern "C" {
#endif

void print_char(char x);
void print_str(char const* x);
void print_newline(void);
void print_set_color(PrintColor foreground, PrintColor background);
void print_set_color_raw(uint8_t color);
void print_ensure_line(void);
void print_clear(void);
// INTEGRAL TYPES
// format for numeric print fns: print_<type>_<format>
// * to print a signed type in hex, bin, or oct, you need to pass it as an unsigned equivalent.
// short
void print_short_dec(short num);
// ushort
void print_ushort_hex(unsigned short num);
void print_ushort_dec(unsigned short num);
void print_ushort_bin(unsigned short num);
void print_ushort_oct(unsigned short num);
// int
void print_int_dec(int num);
// uint
void print_uint_hex(unsigned int num);
void print_uint_dec(unsigned int num);
void print_uint_bin(unsigned int num);
void print_uint_oct(unsigned int num);
// long
void print_long_dec(long num);
// ulong
void print_ulong_hex(unsigned long num);
void print_ulong_dec(unsigned long num);
void print_ulong_oct(unsigned long num);
void print_ulong_bin(unsigned long num);
// llong
void print_llong_dec(long long num);
// ullong
void print_ullong_hex(unsigned long long num);
void print_ullong_dec(unsigned long long num);
void print_ullong_bin(unsigned long long num);
void print_ullong_oct(unsigned long long num);
// char/byte
void print_byte_dec(signed char num);
// uchar/ubyte
void print_ubyte_hex(unsigned char num);
void print_ubyte_dec(unsigned char num);
void print_ubyte_bin(unsigned char num);
void print_ubyte_oct(unsigned char num);
// pointer (only has hex and bin, cast to long/ulong for others)
void print_ptr_hex(void* num);
void print_ptr_bin(void* num);
// OTHER NUMERIC TYPES
// format for these fns: print_<type>
// * only have dec so no suffix
// * floats, doubles, and ldoubles are always signed
// void print_float(float num, size_t precision);
// void print_double(double num, size_t precision);
// void print_ldouble(long double num, size_t precision);

#ifdef __cplusplus
}
#endif
