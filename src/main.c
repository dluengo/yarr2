#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "log.h"
#include "hook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ole");
MODULE_DESCRIPTION("Yarr2 module");
MODULE_VERSION("0.01");

static int __init yarr2_init(void) {
    int err;

    printk(KERN_INFO "Yarr, I'm a mighty pirate!\n");

    // Hook the system call table.
    err = yarr_hook_sct();
    if (err) {
        yarr_log("error calling hook_sct()");
    }

    return 0;
}

static void __exit yarr2_exit(void) {
    printk(KERN_INFO "Keelhaul!\n");
}

module_init(yarr2_init);
module_exit(yarr2_exit);

