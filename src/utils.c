#include <linux/types.h>

#include "utils.h"
#include "log.h"

int lookup_byte(
        unsigned long long *dst,
        unsigned long long src,
        unsigned char byte,
        unsigned int skips)
{
    const int __MAX_ITERS = 500;
    unsigned int i;
    unsigned char *iter;

    if (dst == NULL || src == 0) {
        yarr_log("dst/src pointer is NULL");
        return -1;
    }

    //TODO: The problem is here, casting an integer value into a pointer
    // completely changes the value stored.
    iter = (unsigned char *)src;
    yarr_log("src = 0x%llx, iter = 0x%p", src, iter);

    i = 0;
    while ( (iter[i] != byte && skips != 0) || i < __MAX_ITERS) {
        if (iter[i] == byte) {
            skips--;
        }
        i++;
    }

    if (i >= __MAX_ITERS) {
        yarr_log("reached %d iterations", __MAX_ITERS);
        return -1;
    }

    *dst = (unsigned long long)&(iter[i]);
    return 0;
}

