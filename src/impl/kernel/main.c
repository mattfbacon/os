#include "print.h"
#include "kmsg.h"

void kernel_main() {
	print_clear();
	print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
	print_str("Welcome to 64-bit Land!\n");
	kernel_log_level = KERNEL_LOG;
	klog("test log\n");
	kmsg("test msg\n");
	kinfo("test info\n");
	kwarn("test warn\n");
	kerr("test err\n");
	kcrit("test crit\n");
	kfatal("test fatal\n");
}
