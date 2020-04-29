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

    args.svc = HIDE_PID;
    args.hidepid_args.pid = my_pid;
    err = yarrcall(&args, sizeof(args));
    if (err) {
        printf("Errors hiding\n");
        return -1;
    }

    sleep(15);

    args.svc = UNHIDE_PID;
    err = yarrcall(&args, sizeof(args));
    if (err) {
        printf("Errors stop hiding\n");
    }

    printf("Try again with kill -TERM %d\n", my_pid);
    sleep(15);

    return 0;
}
