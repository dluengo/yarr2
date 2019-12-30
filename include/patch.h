#ifndef __YARR_PATCH
#define __YARR_PATCH

/**
 * Initializes the patch subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int patch_init(void);

/**
 * Frees all the resources associated to the patch subsystem.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int patch_finish(void);

/**
 * Patches the kernel memory at dst with size bytes from src.
 *
 * @dst: The address to be patched.
 * @src: The address with the content to be written.
 * @size: The number of bytes to be read from src and written into dst.
 * @return: Zero on success, non-zero elsewhere.
 */
int patch(void *dst, void *src, size_t size);

/**
 * Undoes a previously patch in a specific address.
 *
 * @addr: The address we want to unpatch.
 * @return: Zero on success, non-zero elsewhere.
 */
int unpatch(void *addr);

/**
 * Undoes all the patches previously done by calling patch().
 *
 * @return: Zero on success, non-zero elsewhere.
 */
int unpatch_all(void);

#endif

