#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>

//#include "yarrcall.h"

/**
 * A very simple program that shows how to request services to yarr2.
 */

#define YARR_VECTOR (184)

int main() {
    int err;

    // yarrcall() receives two parameters, the service index and a pointer to
    // its parameters.
    err = syscall(YARR_VECTOR, 7, NULL);
    if (err) {
        printf("Error invoking yarrcall()\n");
    }

    return 0;
}

