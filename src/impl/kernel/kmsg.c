#include <stdint.h>
#include <stddef.h>
#include "print.h"
#include "kmsg.h"

static const uint8_t KERNEL_PRINT_COLORS[] = {
	0x8, // log: PRINT_COLOR_DARK_GRAY | (PRINT_COLOR_BLACK << 4)
	0x7, // msg: PRINT_COLOR_LIGHT_GRAY | (PRINT_COLOR_BLACK << 4)
	0x9, // info: PRINT_COLOR_LIGHT_BLUE | (PRINT_COLOR_BLACK << 4)
	0xe, // warn: PRINT_COLOR_YELLOW | (PRINT_COLOR_BLACK << 4)
	0xc, // err: PRINT_COLOR_LIGHT_RED | (PRINT_COLOR_BLACK << 4)
	0x4, // crit: PRINT_COLOR_RED | (PRINT_COLOR_BLACK << 4)
	0x40 // fatal: PRINT_COLOR_BLACK | (PRINT_COLOR_RED << 4)
};

static const char* const KERNEL_LOG_LEVEL_NAMES[] = {
	"LOG",
	"MSG",
	"INF",
	"WRN",
	"ERR",
	"CRT",
	"FTL"
};

enum e_loglevel kernel_log_level = KERNEL_MSG;

void kprint(const char* const msg, const size_t msg_level) {
	if (msg_level >= kernel_log_level) {
		print_set_color_raw(KERNEL_PRINT_COLORS[msg_level]);
		print_char('K');
		print_str(KERNEL_LOG_LEVEL_NAMES[msg_level]);
		print_str(": ");
		print_str(msg);
	}
}
