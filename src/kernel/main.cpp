#include "print.hpp"

extern "C" void kernel_main() {
	print_clear();
	print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
	print_str("Welcome to 64-bit Land!\n");
}

extern "C" void test_user_function() {
	print_char('U');
}
