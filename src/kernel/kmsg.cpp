#include <stddef.h>
#include <stdint.h>

#include "kmsg.hpp"
#include "print.hpp"

static uint8_t const KERNEL_PRINT_COLORS[] = {
	PRINT_COLOR_DARK_GRAY | (PRINT_COLOR_BLACK << 4),  // log
	PRINT_COLOR_LIGHT_GRAY | (PRINT_COLOR_BLACK << 4),  // msg
	PRINT_COLOR_LIGHT_BLUE | (PRINT_COLOR_BLACK << 4),  // info
	PRINT_COLOR_YELLOW | (PRINT_COLOR_BLACK << 4),  // warn
	PRINT_COLOR_LIGHT_RED | (PRINT_COLOR_BLACK << 4),  // error
	PRINT_COLOR_RED | (PRINT_COLOR_BLACK << 4),  // crit
	PRINT_COLOR_BLACK | (PRINT_COLOR_RED << 4),  // fatal
};

static char const* const KERNEL_LOG_LEVEL_NAMES[] = { "LOG", "MSG", "INF", "WRN", "ERR", "CRT", "FTL" };

enum e_loglevel kernel_log_level = KERNEL_MSG;

void kprint(char const* const msg, size_t const msg_level) {
	if (msg_level >= kernel_log_level) {
		print_set_color_raw(KERNEL_PRINT_COLORS[msg_level]);
		print_char('K');
		print_str(KERNEL_LOG_LEVEL_NAMES[msg_level]);
		print_str(": ");
		print_str(msg);
	}
}
