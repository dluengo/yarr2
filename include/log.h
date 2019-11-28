#ifndef __YARR_LOG
#define __YARR_LOG

#include <linux/kernel.h>

#ifdef DEBUG
    #define yarr_log(fmt, ...) \
    { \
        printk(KERN_INFO "%s: " fmt "\n", __func__, ##__VA_ARGS__); \
    }

// TODO: These macros actually doesn't work. The kernel writes the newline
// by itself.
    #define yarr_log_nonl(fmt, ...) \
    { \
        printk(KERN_INFO "%s: " fmt, __func__, ##__VA_ARGS__); \
    }

    #define yarr_raw_log(fmt, ...) \
    { \
        printk(KERN_INFO fmt, ##__VA_ARGS__); \
    }
#else
    #define yarr_log(fmt, ...) {}
    #define yarr_log_nonl(fmt, ...) {}
#endif

#endif

