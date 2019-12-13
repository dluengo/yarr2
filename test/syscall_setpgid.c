#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "yarrlib.h"

int main(int argc, char *argv[]) {
    int err;
    YarrcallArgs_t args;

    args.hidepid_args.pid = getpid();
    err = yarrcall(HIDE_PID, &args);
    if (err) {
        printf("Error calling yarrcall HIDE_PID");
        return -1;
    }

    err = setpgid(-1, -1);
    if (!err) {
        printf("-1, -1 no error is an error\n");
    }

    err = setpgid(-1, 0);
    if (!err) {
        printf("-1, 0 no error is an error\n");
    }

    err = setpgid(0, -1);
    if (!err) {
        printf("0, -1 no error is an error\n");
    }

    err = setpgid(0, 0);
    if (err) {
        printf("0, 0 error\n");
    }

    err = setpgid(0, 1);
    if (!err) {
        printf("0, 1 no error is an error\n");
    }

    err = setpgid(1, 0);
    if (!err) {
        printf("1, 0 no error is an error\n");
    }

    err = setpgid(getpid(), getpid());
    if (err) {
        printf("getpid(), getpid() no error is an error\n");
    }

    err = yarrcall(UNHIDE_PID, &args);

    // TODO: Need to create threads and other processes and keep testing other
    // scenarios.

    return 0;
}

