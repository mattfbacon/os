#pragma once

#include "print.hpp"

/**
 * Explanation of log levels:
 * KERNEL_LOG: info only useful for debugging
 * KERNEL_MSG: generic info that could be useful in any context but not warranting special attention
 * KERNEL_INFO: info warranting special attention but not requiring or suggesting intervention
 * KERNEL_WARN: info that suggests an action, for example to facilitate better system stability
 * KERNEL_ERR: info that requires an action to resolve but does not cause the termination of any service
 * KERNEL_CRIT: info that requires an action to resolve and causes the termination of a service
 * KERNEL_FATAL: info that causes the termination of the kernel
 */
enum e_loglevel {
	KERNEL_LOG = 0,
	KERNEL_MSG = 1,
	KERNEL_INFO = 2,
	KERNEL_WARN = 3,
	KERNEL_ERR = 4,
	KERNEL_CRIT = 5,
	KERNEL_FATAL = 6,
};

extern enum e_loglevel kernel_log_level;

void kprint(char const* const msg, size_t const msg_level);

#define klog(msg) (kprint(msg, KERNEL_LOG))
#define kmsg(msg) (kprint(msg, KERNEL_MSG))
#define kinfo(msg) (kprint(msg, KERNEL_INFO))
#define kwarn(msg) (kprint(msg, KERNEL_WARN))
#define kerr(msg) (kprint(msg, KERNEL_ERR))
#define kcrit(msg) (kprint(msg, KERNEL_CRIT))
#define kfatal(msg) (kprint(msg, KERNEL_FATAL))
