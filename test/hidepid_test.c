#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <errno.h>

#include "yarrlib.h"

// TODO: Create common test functionality in a different module.
#define test_log(fmt, ...)                                      \
{                                                               \
    printf("%s: " fmt "\n", __func__, ##__VA_ARGS__); \
}

int __yarr__x64_sys_setpgid_test(void) {
    pid_t pid, new_pid;
    YarrcallArgs_t yarr_args;
    int err = 0, errno_saved;

    // Hide current process.
    pid = getpid();
    err = hide_process(pid);
    if (err) {
        test_log("Error hiding pid %d", pid);
        return -1;
    }

    // Change PGID to our own PGID.
    err = setpgid(pid, pid);
    if (err) {
        test_log("Error setpgid(%d, %d)", pid, pid);
        goto stop_p1;
    }
    
    // Change PGID to our own PGID using 0 as PID.
    err = setpgid(0, pid);
    if (err) {
        test_log("Error setpgid(0, %d)", pid);
        goto stop_p1;
    }
    
    // Change PGID to our own PGID but using 0 as PGID.
    err = setpgid(pid, 0);
    if (err) {
        test_log("Error setpgid(%d, 0)", pid);
        goto stop_p1;
    }

    // Change PGID to our own PGID but using 0 as arguments.
    err = setpgid(0, 0);
    if (err) {
        test_log("Error setpgid(0, 0)");
        goto stop_p1;
    }

    // Change PGID to a non-existant PGID. Note that I just chose a random
    // number, if it turns out there's such a group in the system... well, this
    // test will fail, but I'm feeling lucky :P.
    err = setpgid(pid, 12345);
    if (!err) {
        test_log("Error setpgid(pid, 12345)");
        goto stop_p1;
    }

    // Change the PGID to a existant PGID. Steps are:
    // - Create a new process (inherits the process group of its parent).
    // - Create a new process group for that process.
    // - That new process will try to move back to the current process group.
    //   Note that the current process is hidden.
    new_pid = fork();
    if (new_pid == -1) {
        test_log("Error creating a new process");
        goto stop_p1;
    } else if (new_pid == 0) {

        // Create new process group.
        err = setpgid(0, 0);
        if (err) {
            test_log("Error creating new process group");
            exit(-1);
        }

        // Move back to the previous group. This should fail with ESRCH.
        err = setpgid(0, pid);
        if (!err) {
            test_log("Error expected but not found");
            exit(-1);
        } else if (errno != ESRCH) {
            test_log("Error found but is not ESRCH");
            exit(-1);
        }

        exit(0);
    }

    // Wait for the newly-created process to finish its work.
    err = waitpid(new_pid, NULL, 0);
    if (err == -1) {
        test_log("Error waitpid(new_pid, NULL, 0)");
        goto stop_p1;
    }

stop_p1:
    yarr_args.hidepid_args.pid = getpid();
    err = yarrcall(UNHIDE_PID, &yarr_args);
    if (err) {
        test_log("Error unhiding pid %d", pid);
        return 0;
    }

    return err;
}

int main() {
    int err = 0;

    err = __yarr__x64_sys_setpgid_test();
    if (err) {
        test_log("Errors __yarr__x64_sys_setpgid_test()");
    }

    return err;
}

