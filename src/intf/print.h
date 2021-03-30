#pragma once

#include <stdint.h>
#include <stddef.h>

enum {
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

void print_clear(void);
void print_newline(void);
void print_char(char character);
void print_str(const char* str);

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

void print_set_color(uint8_t fore, uint8_t back);
void print_set_color_raw(uint8_t raw_color);
void print_ensure_line(void);
