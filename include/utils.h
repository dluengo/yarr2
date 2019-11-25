#ifndef __YARR_UTILS
#define __YARR_UTILS

#include <linux/types.h>

// TODO: Document all the functions using Doxygen format.

/**
 * Looks up incrementally for a specific byte from src and return its address
 * or NULL. If skips is greater than 0 it will skip that number of ocurrences.
 *
 * TODO: For now there's a maximum number of iterations to look for the byte.
 * In the future I should do something to remove that.
 */
void* lookup_byte(unsigned char *src, unsigned char byte, unsigned int skips);

#endif
