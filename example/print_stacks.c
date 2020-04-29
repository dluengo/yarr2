#include <stdlib.h>

#include "yarrlib.h"

int main() {
    /* Not really needed. */
    YarrcallArgs_t args;
    args.svc = __SHOW_STACKS;
    yarrcall(&args, sizeof(args));
    return 0;
}
