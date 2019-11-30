#include "yarrcall.h"
#include "log.h"

asmlinkage long entry_yarrcall(struct pt_regs *regs) {
    yarr_log("regs->di = %lx, regs->si = %lx", regs->di, regs->si);
    return yarrcall((int)(regs->di), (void *)(regs->si));
}

long yarrcall(int svc, void *params) {
    yarr_log("service requested %d %px", svc, params);
    return 0;
}


