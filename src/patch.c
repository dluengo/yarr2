#include <asm/paravirt.h>
#include <linux/slab.h>

#include "patch.h"
#include "log.h"
#include "list.h"

// Every time we patch something in the kernel we keep track of the address we
// patch, the amount of bytes we patch an the original content. This way we
// can undo all the patches before unloading.
typedef struct {
    void *addr;
    size_t size;
    void *orig_content;
} __PatchEntry_t;

// The list where we will keep track of all the patches we've done in the
// kernel.
List_t *__patch_list = NULL;

static void __PatchEntry_print(__PatchEntry_t *this) {
    int i;

    if (this == NULL) {
        yarr_log("<NULL>");
        return ;
    }

    yarr_log("__PatchEntry_t: [addr = %px, size = %lu, orig_content = %px]",
            this->addr, this->size, this->orig_content);
    yarr_log_nonl("orig_content: [");
    for (i = 0; i < this->size; i++) {
        yarr_raw_log("%02x, ", ((unsigned char *)this->orig_content)[i]);
    }
    yarr_raw_log(" ]\n");
}

static void __PatchEntry_destroy(__PatchEntry_t *this) {
    if (this != NULL) {
        kfree(this->orig_content);
    }

    kfree(this);
    return;
}

static inline int __patch_initialized(void) {
    return __patch_list == NULL? 0: 0x01ec0ded;
}

static int __update_bookkeeping(
        void *dst,
        void *src,
        size_t size) {
    __PatchEntry_t *patch_entry;
    int err;

    if (dst == NULL || src == NULL || size == 0) {
        yarr_log("dst/src are NULL or size is 0");
        return -1;
    }

    if (!__patch_initialized()) {
        yarr_log("Patch subsytem not initialized");
        return -1;
    }

    // Create a new patch entry for the list.
    patch_entry = kmalloc(sizeof(__PatchEntry_t), GFP_KERNEL);
    if (patch_entry == NULL) {
        yarr_log("Couldn't allocate memory for patch_entry");
        return -1;
    }

    // Initialize the fields of the newly created patch entry.
    patch_entry->addr = dst;
    patch_entry->size = size;
    patch_entry->orig_content = kmalloc(size, GFP_KERNEL);
    if (patch_entry->orig_content == NULL) {
        yarr_log("Couldn't allocate memory for orig_content");
        kfree(patch_entry);
        return -1;
    }

    // Copy the original content that we want to patch so we can restore it
    // in the future. Mind that the original content is still in dst, not src.
    memcpy(patch_entry->orig_content, dst, size);

    // Add the patch entry to the list.
    //
    // TODO: The list is not sorted. We should implement sorted lists and use
    // them.
    err = List_insertData(__patch_list, patch_entry);
    if (err) {
        yarr_log("Error while inserting patch entry into list");
        kfree(patch_entry->orig_content);
        kfree(patch_entry);
        return -1;
    }
    
    return 0;
}

static void __write_with_perms(void *dst, void *src, size_t size) {
    const unsigned long __WP_BIT = 0x0000000000010000;
    unsigned long cr0;

    // Some memory areas are read-only due to the WP bit in CR0, so before
    // writing disable it, then write, then enable it again if it was enable
    // before.
    //
    // TODO: I think we should disable interrupts
    // TODO: We should take some kind of lock, as other CPUs might be trying to
    // read.
    cr0 = read_cr0();
    write_cr0(cr0 & ~__WP_BIT);
    memcpy(dst, src, size);
    write_cr0(cr0 | __WP_BIT);
}

int patch_init(void) {
    // Subsystem was initialized before, end gracefully but raise a message.
    if (__patch_initialized()) {
        yarr_log("Trying to initialize patch subsystem more than once");
        return 0;
    }

    __patch_list = List_create(
            (void *)__PatchEntry_print,
            NULL,
            (void *)__PatchEntry_destroy);
    if (__patch_list == NULL) {
        yarr_log("Error creating __patch_list");
        return -1;
    }

    return 0;
}

int patch_finish(void) {
    if (!__patch_initialized()) {
        return 0;
    }

    // Before stopping the subsystem undo all the patches.
    unpatch_all();
    yarr_log("Patches undone");

    List_destroy(__patch_list);
    __patch_list = NULL;
    return 0;
}

int patch(void *dst, void *src, size_t size) {
    int err;

    if (dst == NULL || src == NULL || size == 0) {
        yarr_log("dst/src are NULL or size is 0");
        return -1;
    }

    if (!__patch_initialized()) {
        yarr_log("Patch subsystem is not initialized");
        return -1;
    }

    err = __update_bookkeeping(dst, src, size);
    if (err) {
        yarr_log("Couldn't update the bookkeeping");
        return -1;
    }

    __write_with_perms(dst, src, size);
    return 0;
}

int unpatch(void *addr) {
    __PatchEntry_t *patch_entry;
    int i;

    if (addr == NULL) {
        yarr_log("Given address is NULL");
        return -1;
    }

    if (!__patch_initialized()) {
        yarr_log("Patch subsystem is not initialized");
        return -1;
    }

    for (i = 0; i < List_length(__patch_list); i++) {
        patch_entry = (__PatchEntry_t *)List_getData(__patch_list, i);
        if (patch_entry == NULL) {
            yarr_log("patch_entry is NULL, in theory this is not possible!");
            continue;
        }

        if (patch_entry->addr == addr) {
            __write_with_perms(patch_entry->addr,
                    patch_entry->orig_content,
                    patch_entry->size);
            return 0;
        }
    }

    return -1;
}

int unpatch_all(void) {
    __PatchEntry_t *patch_entry;
    unsigned int i;

    if (!__patch_initialized()) {
        return 0;
    }

    for (i = 0; i < List_length(__patch_list); i++) {
        patch_entry = (__PatchEntry_t *)List_getData(__patch_list, i);
        if (patch_entry == NULL) {
            yarr_log("patch_entry is NULL, this is not possible in theory");
            continue;
        }

        __write_with_perms(patch_entry->addr,
                patch_entry->orig_content,
                patch_entry->size);
    }

    return 0;
}

