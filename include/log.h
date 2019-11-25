#ifndef __YARR_LOG__
#define __YARR_LOG__

#include <linux/kernel.h>

#ifdef DEBUG
    #define yarr_log(fmt, ...) \
    { \
        printk(KERN_INFO "%s: " fmt "\n", __func__, ##__VA_ARGS__); \
    }
#else
    #define yarr_log(fmt, ...) {}
#endif

#endif

