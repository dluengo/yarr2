#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "yarrlib.h"

int main() {
    YarrcallArgs_t *args;
    int fd, err, fd_aux, errno_saved;
    char fname[] = "testfile_XXXXXX";
    size_t fname_len = strlen(fname);
    size_t args_size;

    /* Need an array as mkstemp() modifies the XXXXXX part. */
    fd = mkstemp(fname);
    if (fd < 0) {
        errno_saved = errno;
        printf("Error creating temporary file %d\n", errno_saved);
        return -1;
    }
    close(fd);

    args_size = sizeof(YarrcallArgs_t) - sizeof(union args) + sizeof(struct hidefile_args) + fname_len + 1;
    args = malloc(args_size);
    if (args == NULL) {
        printf("Couldn't allocate memory for arguments");
        return -1;
    }

    args->svc = HIDE_FILE;
    args->hidefile_args.size = fname_len;
    strncpy(args->hidefile_args.fname, fname, fname_len);

    printf("Going to hide file %s\n", args->hidefile_args.fname);
    err = yarrcall(args, args_size);
    if (err) {
        printf("Errors hiding the file\n");
        free(args);
        return -1;
    }

    fd_aux = open(fname, 0400);
    if (fd_aux > -1) {
        printf("We could open the file, we shouldn't be allowed\n");
        unlink(fname);
        free(args);
        return -1;
    }

    args->svc = UNHIDE_FILE;
    err = yarrcall(args, sizeof(args));
    if (err) {
        printf("Errors unhiding the file");
    }

    unlink(fname);
    YarrcallArgs_free(args);
    return 0;
}

