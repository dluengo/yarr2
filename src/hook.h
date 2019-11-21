#ifndef __YARR_HOOK
#define __YARR_HOOK

/**
 * Hooks the system call tables.
 *
 * In x86 Linux there could be up to two system call tables. One is
 * named sys_call_table[] for 64-bit syscalls, and ia32_sys_call_table[] for
 * 32-bit syscalls. For further documentation read doc/linux_system_calls.txt
 */
int hook_syscall_tables(void);

#endif
