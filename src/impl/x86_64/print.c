#include "print.h"

#include <stdbool.h>

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

	for (size_t row = 1; row < NUM_ROWS; row++) {
		for (size_t col = 0; col < NUM_COLS; col++) {
			buffer[col + NUM_COLS * (row - 1)] = buffer[col + NUM_COLS * row];
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

void print_str(char* str) {
	for (; *str != '\0'; str += sizeof(char)) {
		print_char(*str);
	}
}

void print_set_color(uint8_t fore, uint8_t back) {
	color = fore | (back << 4);
}
