#ifndef __YARR_UTILS
#define __YARR_UTILS

#include <linux/types.h>

/**
 * Looks up incrementally for a specific byte from src and puts its address in
 * dst. If skips is greater than 0 it will skip that number of ocurrences.
 */
int lookup_byte(
        unsigned long long *dst,
        unsigned long long src,
        unsigned char byte,
        unsigned int skips);

#endif
