#include <linux/types.h>

#include "utils.h"
#include "log.h"

// TODO: Document all the functions using Doxygen format.

void* lookup_byte(unsigned char *src, unsigned char byte, unsigned int skips) {
    const int __MAX_ITERS = 500;
    unsigned int i;

    if (src == NULL) {
        yarr_log("src pointer is NULL");
        return NULL;
    }

    i = 0;
    while ( (src[i] != byte || skips != 0) && i < __MAX_ITERS) {

        // We found a byte with the value we are looking for and we don't need
        // to skip any more bytes, so get out of the loop.
        if (src[i] == byte && skips == 0) {
            break;
        }

        // We found the byte we are looking for, but we have to skip it.
        if (src[i] == byte) {
            skips--;
        }

        i++;
    }

    if (i >= __MAX_ITERS) {
        yarr_log("reached %d iterations", __MAX_ITERS);
        return NULL;
    }

    return &(src[i]);
}

