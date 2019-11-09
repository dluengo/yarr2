#ifndef __YARR_LOG
#define __YARR_LOG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#ifdef DEBUG
    #define yarr_log(msg) { printk(KERN_INFO "%s: %s", __func__, msg); }
#else
    #define yarr_log(msg) {;}
#endif

#endif
