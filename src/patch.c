#include <asm/paravirt.h>
#include <linux/slab.h>
#include <linux/rbtree.h>

#include "patch.h"
#include "log.h"

// Every time we patch something in the kernel we keep track of the address we
// patch, the amount of bytes we patch an the original content. This way we
// can undo all the patches before unloading.
typedef struct __patch_entry {
    struct rb_node node;

    void *addr;
    size_t size;
    void *orig_content;
} __PatchEntry_t;

// The list where we will keep track of all the patches we've done in the
// kernel.
struct rb_root __patch_list = RB_ROOT;

static int __PatchEntry_init(
        __PatchEntry_t *this,
        void *addr,
        size_t size) {
    if (this == NULL) {
        yarr_log("this is NULL");
        return -1;
    }

    this->node.rb_right = NULL;
    this->node.rb_left = NULL;
    this->addr = addr;
    this->size = size;
    this->orig_content = kmalloc(size, GFP_KERNEL);
    if (this->orig_content == NULL) {
        yarr_log("Couldn't allocate memory for orig_content");
        return -1;
    }

    memcpy(this->orig_content, addr, size);
    return 0;
}

static void __PatchEntry_free(__PatchEntry_t *this) {
    if (this != NULL) {
        kfree(this->orig_content);
    }

    kfree(this);
    return;
}

static __PatchEntry_t * __PatchEntry_search(struct rb_root *root, void *addr) {
    __PatchEntry_t *data;
    struct rb_node *iter;

    if (root == NULL || addr == NULL) {
        yarr_log("NULL argument");
        return NULL;
    }

    iter = root->rb_node;
    while (iter != NULL) {
        data = rb_entry(iter, __PatchEntry_t, node);
        if (addr < data->addr) {
            iter = iter->rb_left;
        } else if (addr > data->addr) {
            iter = iter->rb_right;
        } else {
            return data;
        }
    }

    return NULL;
}

#define __YARR_PATCH_ELEMENT_PRESENT (2)
static int __PatchEntry_insert(struct rb_root *root, __PatchEntry_t *patch) {
    __PatchEntry_t *entry;
    struct rb_node **link, *parent;

    if (root == NULL || patch == NULL) {
        yarr_log("NULL argument");
        return -1;
    }

    link = &(root->rb_node);
    parent = NULL;
    while (*link != NULL) {
        parent = *link;
        entry = container_of(*link, __PatchEntry_t, node);

        if (patch->addr < entry->addr) {
            link = &((*link)->rb_left);
        } else if (patch->addr > entry->addr) {
            link = &((*link)->rb_right);
        } else {
            return -__YARR_PATCH_ELEMENT_PRESENT;
        }
    }

    rb_link_node(&patch->node, parent, link);
    rb_insert_color(&patch->node, root);
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
    return 0;
}

int patch_finish(void) {
    unpatch_all();
    return 0;
}

int patch(void *dst, void *src, size_t size) {
    __PatchEntry_t *new_patch;
    int err;

    if (dst == NULL || src == NULL || size == 0) {
        yarr_log("dst/src are NULL or size is 0");
        return -1;
    }

    new_patch = kmalloc(sizeof(__PatchEntry_t), GFP_KERNEL);
    if (new_patch == NULL) {
        yarr_log("Couldn't allocate memory for new patch");
        return -1;
    }

    err = __PatchEntry_init(new_patch, dst, size);
    if (err) {
        yarr_log("Error initializing new patch entry");
        kfree(new_patch);
        return -1;
    }

    err = __PatchEntry_insert(&__patch_list, new_patch);
    if (err) {
        if (err == -__YARR_PATCH_ELEMENT_PRESENT) {
            yarr_log("Applying patch to a already-patched address?");
        } else {
            yarr_log("Error inserting patch in the tree");
        }

        return -1;
    }

    __write_with_perms(dst, src, size);
    return 0;
}

int unpatch(void *addr) {
    __PatchEntry_t *patch_entry;
    struct rb_node *victim;

    if (addr == NULL) {
        yarr_log("Given address is NULL");
        return -1;
    }

    patch_entry = __PatchEntry_search(&__patch_list, addr);
    if (patch_entry == NULL) {
        yarr_log("Patch address 0x%px not patched before", addr);
        return -1;
    }

    victim = &patch_entry->node;
    rb_erase(victim, &__patch_list);

    __write_with_perms(patch_entry->addr,
            patch_entry->orig_content,
            patch_entry->size);

    __PatchEntry_free(patch_entry);
    return 0;
}

int unpatch_all(void) {
    __PatchEntry_t *patch_entry;
    struct rb_node *node;

    node = rb_first(&__patch_list);
    while (node != NULL) {
        patch_entry = rb_entry(node, __PatchEntry_t, node);
        if (patch_entry == NULL) {
            yarr_log("rb_node not NULL and entry NULL, not possible!!!");
            return -1;
        }

        rb_erase(node, &__patch_list);

        __write_with_perms(patch_entry->addr,
                patch_entry->orig_content,
                patch_entry->size);

        node = rb_next(node);
        __PatchEntry_free(patch_entry);
    }

    return 0;
}

