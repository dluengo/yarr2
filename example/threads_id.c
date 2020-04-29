#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <asm/unistd.h>

#include "yarrlib.h"

long gettid() {
    return syscall(__NR_gettid);
}

void * thread_main(void *args) {
    pid_t pid, pgid;
    long tid;
    YarrcallArgs_t proc_info;

    pid = getpid();
    pgid = getpgid(pid);
    tid = gettid();

    proc_info.svc = __GET_PROC_INFO;
    yarrcall(&proc_info, sizeof(proc_info));

    printf("PID  is          0x%016x\n", pid);
    printf("PGID is          0x%016x\n", pgid);
    printf("TID  is          0x%016lx\n", tid);
    printf("Yarrcall pid is  0x%016x\n", proc_info.getprocinfo_args.pid);
    printf("Yarrcall tgid is 0x%016x\n", proc_info.getprocinfo_args.tgid);
    printf("\n");

    return NULL;
}

int main() {
    pid_t pid, pgid;
    long tid;

    pthread_attr_t t_attr;
    pthread_t pthread_id;

    YarrcallArgs_t args;

    pid = getpid();
    pgid = getpgid(pid);
    tid = gettid();

    args.svc = __GET_PROC_INFO;
    yarrcall(&args, sizeof(args));

    printf("PID  is          0x%016x\n", pid);
    printf("PGID is          0x%016x\n", pgid);
    printf("TID  is          0x%016lx\n", tid);
    printf("Yarrcall pid is  0x%016x\n", args.getprocinfo_args.pid);
    printf("Yarrcall tgid is 0x%016x\n", args.getprocinfo_args.tgid);
    printf("\n");

    pthread_attr_init(&t_attr);
    pthread_create(&pthread_id, &t_attr, thread_main, NULL);
    printf("Thread 0x%016lx created\n", pthread_id);
    pthread_join(pthread_id, NULL);

    return 0;
}

