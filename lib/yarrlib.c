#define _GNU_SOURCE
#include <unistd.h>

#include "yarrlib.h"

long yarrcall(int svc, YarrcallArgs_t *args) {
    return syscall(YARR_VECTOR, svc, args);
}

