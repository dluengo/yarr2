#ifndef __YARR_UTILS
#define __YARR_UTILS

#include <linux/types.h>

/**
 * Looks up incrementally for a specific byte from src and return its address
 * or NULL. If skips is greater than 0 it will skip that number of ocurrences.
 *
 * TODO: For now there's a maximum number of iterations to look for the byte.
 * In the future we should remove that, but for now I found it more helpful
 * than harmful, so it stays.
 */
void* lookup_byte(unsigned char *src, unsigned char byte, unsigned int skips);

/**
 * Return the lower 4 bytes of a 8-byte value.
 *
 * @src: The 8-byte value from where to get the lower 4 bytes.
 * @return: Not clear enough? The lower 4 bytes of the value passed.
 */
int32_t get_low_4_bytes(unsigned long src);

#endif
