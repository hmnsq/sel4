/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/fastpath/fastpath.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>

#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ VISIBLE NORETURN restore_user_context(ଶୂନ୍ୟ)
{
    word_t cur_thread_reg = (word_t) ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#ifdef ENABLE_SMP_SUPPORT
    word_t sp = read_sscratch();
    sp -= ଆକାର(word_t);
    *((word_t *)sp) = cur_thread_reg;
#endif

#ifdef CONFIG_HAVE_FPU
    set_tcb_fs_state(ଗଣ୍ଠି_STATE(ksCurThread), isFpuEnable());
#endif

    ଗଣ୍ଠି_UNLOCK_IF_HELD;

    asm ଅସ୍ଥିର(
        "mv t0, %[cur_thread]       \n"
        LOAD_S " ra, (0*%[REGSIZE])(t0)  \n"
        LOAD_S "  sp, (1*%[REGSIZE])(t0)  \n"
        LOAD_S "  gp, (2*%[REGSIZE])(t0)  \n"
        LOAD_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        LOAD_S "  t2, (6*%[REGSIZE])(t0)  \n"
        LOAD_S "  s0, (7*%[REGSIZE])(t0)  \n"
        LOAD_S "  s1, (8*%[REGSIZE])(t0)  \n"
        LOAD_S "  a0, (9*%[REGSIZE])(t0) \n"
        LOAD_S "  a1, (10*%[REGSIZE])(t0) \n"
        LOAD_S "  a2, (11*%[REGSIZE])(t0) \n"
        LOAD_S "  a3, (12*%[REGSIZE])(t0) \n"
        LOAD_S "  a4, (13*%[REGSIZE])(t0) \n"
        LOAD_S "  a5, (14*%[REGSIZE])(t0) \n"
        LOAD_S "  a6, (15*%[REGSIZE])(t0) \n"
        LOAD_S "  a7, (16*%[REGSIZE])(t0) \n"
        LOAD_S "  s2, (17*%[REGSIZE])(t0) \n"
        LOAD_S "  s3, (18*%[REGSIZE])(t0) \n"
        LOAD_S "  s4, (19*%[REGSIZE])(t0) \n"
        LOAD_S "  s5, (20*%[REGSIZE])(t0) \n"
        LOAD_S "  s6, (21*%[REGSIZE])(t0) \n"
        LOAD_S "  s7, (22*%[REGSIZE])(t0) \n"
        LOAD_S "  s8, (23*%[REGSIZE])(t0) \n"
        LOAD_S "  s9, (24*%[REGSIZE])(t0) \n"
        LOAD_S "  s10, (25*%[REGSIZE])(t0)\n"
        LOAD_S "  s11, (26*%[REGSIZE])(t0)\n"
        LOAD_S "  t3, (27*%[REGSIZE])(t0) \n"
        LOAD_S "  t4, (28*%[REGSIZE])(t0) \n"
        LOAD_S "  t5, (29*%[REGSIZE])(t0) \n"
        LOAD_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        LOAD_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#ifndef ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#endif
        LOAD_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        LOAD_S "  t1, (5*%[REGSIZE])(t0) \n"
        LOAD_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(ଆକାର(word_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_interrupt(ଶୂନ୍ୟ)
{
    ଗଣ୍ଠି_LOCK_IRQ_IF(getActiveIRQ() != irq_remote_call_ipi);

    c_entry_hook();

    handleInterruptEntry();

    restore_user_context();
    UNREACHABLE();
}

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_exception(ଶୂନ୍ୟ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    ଯଦି (read_sstatus() & SSTATUS_SPP) {
#ifdef CONFIG_DEBUG_BUILD
        printf("\n\nKERNEL ABORT (exception within s-mode)!\n");
        printf("scause: 0x%"SEL4_PRIx_word", stval: 0x%"SEL4_PRIx_word"\n",
               read_scause(), read_stval());
#endif
        halt();
    }

    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();

    word_t scause = read_scause();
    ବାଛ (scause) {
    ସ୍ଥିତି RISCVInstructionAccessFault:
    ସ୍ଥିତି RISCVLoadAccessFault:
    ସ୍ଥିତି RISCVStoreAccessFault:
    ସ୍ଥିତି RISCVLoadPageFault:
    ସ୍ଥିତି RISCVStorePageFault:
    ସ୍ଥିତି RISCVInstructionPageFault:
        handleVMFaultEvent(scause);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        handleUserLevelFault(scause, 0);
        ଭାଙ୍ଗ;
    }

    restore_user_context();
    UNREACHABLE();
}

ଶୂନ୍ୟ VISIBLE NORETURN slowpath(syscall_t syscall)
{
    ଯଦି (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ଅନ୍ୟଥା {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE)
#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ସନ୍ଦେଶ_Info, word_t reply)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t ସନ୍ଦେଶ_Info)
#endif
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info, reply);
#ଅନ୍ୟଥା
    fastpath_reply_recv(cptr, ସନ୍ଦେଶ_Info);
#endif
    UNREACHABLE();
}

ALIGN(L1_CACHE_ଧାଡ଼ି_SIZE)
ଶୂନ୍ୟ VISIBLE c_handle_fastpath_call(word_t cptr, word_t ସନ୍ଦେଶ_Info)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, ସନ୍ଦେଶ_Info);

    UNREACHABLE();
}
#endif

ଶୂନ୍ୟ VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t ସନ୍ଦେଶ_Info, syscall_t syscall)
{
    ଗଣ୍ଠି_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, ସନ୍ଦେଶ_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
