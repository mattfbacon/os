#pragma once

inline constexpr auto ones(auto const n_bits) {
	return (1 << n_bits) - 1;
}
inline constexpr auto bit_test(auto const bit) {
	return 1 << (bit - 1);
}

static constexpr auto const HEX_MASK = 0xf;
static constexpr auto const HEX_BITS = 4;  // 4 bits per hex digit
static constexpr auto const BYTE_BITS = 8;  // 8 bits per byte
static constexpr auto const SHORT_BITS = (sizeof(short) * BYTE_BITS);
static constexpr auto const INT_BITS = (sizeof(int) * BYTE_BITS);
static constexpr auto const LONG_BITS = (sizeof(long) * BYTE_BITS);
static constexpr auto const LLONG_BITS = (sizeof(long long) * BYTE_BITS);
static constexpr auto const FLOAT_BITS = (sizeof(float) * BYTE_BITS);
static constexpr auto const DOUBLE_BITS = (sizeof(double) * BYTE_BITS);
static constexpr auto const LDOUBLE_BITS = (sizeof(long double) * BYTE_BITS);
static constexpr auto const PTR_BITS = 64;  // number of bits in a pointer
constexpr auto EXTRA_HEX_BITS(auto size) {  // number of bits after the top four
	return size - HEX_BITS;
}
