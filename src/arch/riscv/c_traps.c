/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <arch/fastpath/fastpath.h>
#അന്തര്ഭാവ <arch/kernel/traps.h>
#അന്തര്ഭാവ <machine/debug.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <arch/machine/hardware.h>
#അന്തര്ഭാവ <machine/fpu.h>

#അന്തര്ഭാവ <benchmark/benchmark_track.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ശൂന്യം VISIBLE NORETURN restore_user_context(ശൂന്യം)
{
    word_t cur_thread_reg = (word_t) കണ്ണി_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#ifdef ENABLE_SMP_SUPPORT
    word_t sp = read_sscratch();
    sp -= വലുപ്പം(word_t);
    *((word_t *)sp) = cur_thread_reg;
#endif

#ifdef CONFIG_HAVE_FPU
    set_tcb_fs_state(കണ്ണി_STATE(ksCurThread), isFpuEnable());
#endif

    കണ്ണി_UNLOCK_IF_HELD;

    asm അസ്ഥിര(
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
        : [REGSIZE] "i"(വലുപ്പം(word_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

ശൂന്യം VISIBLE NORETURN c_handle_interrupt(ശൂന്യം)
{
    കണ്ണി_LOCK_IRQ_IF(getActiveIRQ() != irq_remote_call_ipi);

    c_entry_hook();

    handleInterruptEntry();

    restore_user_context();
    UNREACHABLE();
}

ശൂന്യം VISIBLE NORETURN c_handle_exception(ശൂന്യം)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    എങ്കിൽ (read_sstatus() & SSTATUS_SPP) {
#ifdef CONFIG_DEBUG_BUILD
        printf("\n\nKERNEL ABORT (exception within s-mode)!\n");
        printf("scause: 0x%"SEL4_PRIx_word", stval: 0x%"SEL4_PRIx_word"\n",
               read_scause(), read_stval());
#endif
        halt();
    }

    കണ്ണി_LOCK_SYS;

    c_entry_hook();

    word_t scause = read_scause();
    തിരഞ്ഞെടുപ്പ് (scause) {
    അവസ്ഥ RISCVInstructionAccessFault:
    അവസ്ഥ RISCVLoadAccessFault:
    അവസ്ഥ RISCVStoreAccessFault:
    അവസ്ഥ RISCVLoadPageFault:
    അവസ്ഥ RISCVStorePageFault:
    അവസ്ഥ RISCVInstructionPageFault:
        handleVMFaultEvent(scause);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        handleUserLevelFault(scause, 0);
        നിർത്തുക;
    }

    restore_user_context();
    UNREACHABLE();
}

ശൂന്യം VISIBLE NORETURN slowpath(syscall_t syscall)
{
    എങ്കിൽ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.path = Entry_UnknownSyscall;
#endif /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } അല്ലെങ്കിൽ {
#ifdef TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = 0;
#endif /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ifdef CONFIG_FASTPATH
ALIGN(L1_CACHE_വരി_SIZE)
#ifdef CONFIG_KERNEL_MCS
ശൂന്യം VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t സന്ദേശം_Info, word_t reply)
#അല്ലെങ്കിൽ
ശൂന്യം VISIBLE c_handle_fastpath_reply_recv(word_t cptr, word_t സന്ദേശം_Info)
#endif
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, SysReplyRecv);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */
#ifdef CONFIG_KERNEL_MCS
    fastpath_reply_recv(cptr, സന്ദേശം_Info, reply);
#അല്ലെങ്കിൽ
    fastpath_reply_recv(cptr, സന്ദേശം_Info);
#endif
    UNREACHABLE();
}

ALIGN(L1_CACHE_വരി_SIZE)
ശൂന്യം VISIBLE c_handle_fastpath_call(word_t cptr, word_t സന്ദേശം_Info)
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, SysCall);
    ksKernelEntry.is_fastpath = 1;
#endif /* DEBUG */

    fastpath_call(cptr, സന്ദേശം_Info);

    UNREACHABLE();
}
#endif

ശൂന്യം VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t സന്ദേശം_Info, syscall_t syscall)
{
    കണ്ണി_LOCK_SYS;

    c_entry_hook();
#ifdef TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, സന്ദേശം_Info, syscall);
    ksKernelEntry.is_fastpath = 0;
#endif /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
