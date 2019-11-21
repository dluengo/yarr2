#include <linux/kernel.h>
//#include <linux/syscalls.h>
//#include <asm/irq_vectors.h>
//#include <asm/syscall.h>
//#include <asm/desc.h>
//#include <asm/desc_defs.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/msr.h>

#include "hook.h"
#include "log.h"
#include "utils.h"

int __hook_syscall_table_64(void) {
    //unsigned char *entry_SYSCALL_64;
    unsigned char *do_syscall_64;
    unsigned long long addr, entry_SYSCALL_64;
    int32_t displacement;
    int err;

    // Get the address of the LSTAR MSR, it contains the entry point of
    // instruction syscall.
    //rdmsrl(MSR_LSTAR, addr);
    //entry_SYSCALL_64 = (unsigned char *)addr;
    rdmsrl(MSR_LSTAR, entry_SYSCALL_64);

    //yarr_log("addr is at 0x%p", &addr);
    yarr_log("entry_SYSCALL_64 is at 0x%p", &entry_SYSCALL_64);
    //yarr_log("MSR_LSTAR is %lx", addr);
    yarr_log("entry_SYSCALL_64 0x%llx", entry_SYSCALL_64);

    if (entry_SYSCALL_64 == 0) {
        yarr_log("error retrieving MSR_LSTAR address");
        return -1;
    }

    // Look for the first e8 byte. This byte is the opcode for a
    // near/relative call instruction.
    err = lookup_byte(&addr, entry_SYSCALL_64, '\xe8', 0);
    if (err) {
        yarr_log("error while looking up call instruction opcode.");
        return -1;
    }

    yarr_log("Call opcode is at %p", (void *)addr);

    // Get the next 4 bytes from the call instruction (displacement).
    memcpy(&displacement, (void *)(addr + 1), 4);

    // Calculate the address of do_syscall_64().
    do_syscall_64 = (void *)(addr + 5 + displacement);

    yarr_log("do_syscall_64() is at addr %p", do_syscall_64);

    return 0;
}

int hook_syscall_tables(void) {
    int err;

    err = __hook_syscall_table_64();
    if (err) {
        yarr_log("error hooking sys_call_table for 64-bit");
    }

    return 0;
}

