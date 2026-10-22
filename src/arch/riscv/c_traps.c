/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <model/statedata.h>
#समावेश <arch/fastpath/fastpath.h>
#समावेश <arch/kernel/traps.h>
#समावेश <machine/debug.h>
#समावेश <api/syscall.h>
#समावेश <util.h>
#समावेश <arch/machine/hardware.h>
#समावेश <machine/fpu.h>

#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
व्योम VISIBLE NORETURN restore_user_context(व्योम)
{
    word_t cur_thread_reg = (word_t) गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#अगर_घोषित ENABLE_SMP_SUPPORT
    word_t sp = read_sscratch();
    sp -= माप(word_t);
    *((word_t *)sp) = cur_thread_reg;
#पूर्ण_अगर

#अगर_घोषित CONFIG_HAVE_FPU
    set_tcb_fs_state(गाँठ_STATE(ksCurThread), isFpuEnable());
#पूर्ण_अगर

    गाँठ_UNLOCK_IF_HELD;

    यंत्र अस्थिर(
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
#अगर_अघोषित ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#पूर्ण_अगर
        LOAD_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        LOAD_S "  t1, (5*%[REGSIZE])(t0) \n"
        LOAD_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(माप(word_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

व्योम VISIBLE NORETURN c_handle_interrupt(व्योम)
{
    गाँठ_LOCK_IRQ_IF(getActiveIRQ() != irq_remote_call_ipi);

    c_entry_hook();

    handleInterruptEntry();

    restore_user_context();
    UNREACHABLE();
}

व्योम VISIBLE NORETURN c_handle_exception(व्योम)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    अगर (read_sstatus() & SSTATUS_SPP) {
#अगर_घोषित CONFIG_DEBUG_BUILD
        म_लिखो("\n\nKERNEL ABORT (exception within s-mode)!\n");
        म_लिखो("scause: 0x%"SEL4_PRIx_word", stval: 0x%"SEL4_PRIx_word"\n",
               read_scause(), read_stval());
#पूर्ण_अगर
        halt();
    }

    गाँठ_LOCK_SYS;

    c_entry_hook();

    word_t scause = read_scause();
    चयन (scause) {
    हाल RISCVInstructionAccessFault:
    हाल RISCVLoadAccessFault:
    हाल RISCVStoreAccessFault:
    हाल RISCVLoadPageFault:
    हाल RISCVStorePageFault:
    हाल RISCVInstructionPageFault:
        handleVMFaultEvent(scause);
        अवरोध;
    शेष:
        handleUserLevelFault(scause, 0);
        अवरोध;
    }

    restore_user_context();
    UNREACHABLE();
}

व्योम VISIBLE NORETURN slowpath(syscall_t syscall)
{
    अगर (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
#पूर्ण_अगर /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#अगर_घोषित TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#पूर्ण_अगर /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#अगर_घोषित CONFIG_FASTPATH
ALIGN(L1_CACHE_पंक्ति_आकार)
#अगर_घोषित CONFIG_KERNEL_MCS
व्योम VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info, word_t reply)
#अन्यथा
व्योम VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t संदेश_Info)
#पूर्ण_अगर
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* DEBUG */
#अगर_घोषित CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, संदेश_Info, reply);
#अन्यथा
    fastpath_reply_recv(cptr, संदेश_Info);
#पूर्ण_अगर
    UNREACHABLE();
}

ALIGN(L1_CACHE_पंक्ति_आकार)
व्योम VISIBLE c_handle_fastpath_call(word_t cptr, word_t संदेश_Info)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#पूर्ण_अगर /* DEBUG */

    fastpath_call(cptr, संदेश_Info);

    UNREACHABLE();
}
#पूर्ण_अगर

व्योम VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t संदेश_Info, syscall_t syscall)
{
    गाँठ_LOCK_SYS;

    c_entry_hook();
#अगर_घोषित TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, संदेश_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#पूर्ण_अगर /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
