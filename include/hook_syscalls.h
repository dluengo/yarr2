#ifndef __YARR_HOOK_SYSCALLS
#define __YARR_HOOK_SYSCALLS

/**
 * Hooks the system calls that yarr2 is interested into. This hook takes place
 * in the fake_sct, as in theory that's the syscall table that we are going to
 * provide to the system. It also needs the original syscall table in order for
 * our hooking functions to be able to call the original syscalls, as the
 * symbols of the syscalls usually with the form of "__x64_sys_<name>()" (in
 * 64-bit system) are not exported by the kernel.
 *
 * This function is suppossed to be called just once (singleton), if it detects
 * it has been called twice it returns with error.
 *
 * @fake_sct: A pointer to the fake syscall table that yarr2 creates and where
 * we will store the hooking functions.
 * @orig_sct: A pointer to the original syscall table of the system, needed to
 * look for the original syscalls.
 * @return: Zero on success, non-zero elsewhere.
 */
int hook_syscalls(unsigned long *fake_sct, unsigned long *orig_sct);

#endif
