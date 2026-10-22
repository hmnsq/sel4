/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/fastpath/fastpath.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <machine/debug.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <arch/machine/hardware.h>
#अन्तर्भाव <machine/fpu.h>

#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
शून्य VISIBLE NORETURN restore_user_context(शून्य)
{
    word_t cur_thread_reg = (word_t) ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    word_t sp = read_sscratch();
    sp -= परिमाण(word_t);
    *((word_t *)sp) = cur_thread_reg;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    समुच्चयः_tcb_fs_state(ग्रन्थिः_STATE(ksCurThread), isFpuEnable());
#यदि_अन्तः

    ग्रन्थिः_UNLOCK_IF_HELD;

    asm चञ्चल(
        "mv t0, %[cur_thread]       \n"
        आरोपय_S " ra, (0*%[REGSIZE])(t0)  \n"
        आरोपय_S "  sp, (1*%[REGSIZE])(t0)  \n"
        आरोपय_S "  gp, (2*%[REGSIZE])(t0)  \n"
        आरोपय_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        आरोपय_S "  t2, (6*%[REGSIZE])(t0)  \n"
        आरोपय_S "  s0, (7*%[REGSIZE])(t0)  \n"
        आरोपय_S "  s1, (8*%[REGSIZE])(t0)  \n"
        आरोपय_S "  a0, (9*%[REGSIZE])(t0) \n"
        आरोपय_S "  a1, (10*%[REGSIZE])(t0) \n"
        आरोपय_S "  a2, (11*%[REGSIZE])(t0) \n"
        आरोपय_S "  a3, (12*%[REGSIZE])(t0) \n"
        आरोपय_S "  a4, (13*%[REGSIZE])(t0) \n"
        आरोपय_S "  a5, (14*%[REGSIZE])(t0) \n"
        आरोपय_S "  a6, (15*%[REGSIZE])(t0) \n"
        आरोपय_S "  a7, (16*%[REGSIZE])(t0) \n"
        आरोपय_S "  s2, (17*%[REGSIZE])(t0) \n"
        आरोपय_S "  s3, (18*%[REGSIZE])(t0) \n"
        आरोपय_S "  s4, (19*%[REGSIZE])(t0) \n"
        आरोपय_S "  s5, (20*%[REGSIZE])(t0) \n"
        आरोपय_S "  s6, (21*%[REGSIZE])(t0) \n"
        आरोपय_S "  s7, (22*%[REGSIZE])(t0) \n"
        आरोपय_S "  s8, (23*%[REGSIZE])(t0) \n"
        आरोपय_S "  s9, (24*%[REGSIZE])(t0) \n"
        आरोपय_S "  s10, (25*%[REGSIZE])(t0)\n"
        आरोपय_S "  s11, (26*%[REGSIZE])(t0)\n"
        आरोपय_S "  t3, (27*%[REGSIZE])(t0) \n"
        आरोपय_S "  t4, (28*%[REGSIZE])(t0) \n"
        आरोपय_S "  t5, (29*%[REGSIZE])(t0) \n"
        आरोपय_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        आरोपय_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#यदि_अपरिभाषितम् ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#यदि_अन्तः
        आरोपय_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        आरोपय_S "  t1, (5*%[REGSIZE])(t0) \n"
        आरोपय_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(परिमाण(word_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

शून्य VISIBLE NORETURN c_handle_व्यवधानम्(शून्य)
{
    ग्रन्थिः_तालकम्_IRQ_IF(get_सक्रिय_IRQ() != irq_remote_call_ipi);

    c_प्रविष्टिः_hook();

    handle_व्यवधानम्_प्रविष्टिः();

    restore_user_context();
    UNREACHABLE();
}

शून्य VISIBLE NORETURN c_handle_exception(शून्य)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    यदि (read_sstatus() & SSTATUS_SPP) {
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
        प्रारूप_लिख("\n\nKERNEL ABORT (exception within s-mode)!\n");
        प्रारूप_लिख("scause: 0x%"SEL4_PRIx_word", stval: 0x%"SEL4_PRIx_word"\n",
               read_scause(), read_stval());
#यदि_अन्तः
        halt();
    }

    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();

    word_t scause = read_scause();
    विकल्प (scause) {
    पक्ष RISCVInstructionAccessFault:
    पक्ष RISCVLoadAccessFault:
    पक्ष RISCVStoreAccessFault:
    पक्ष RISCVLoadPageFault:
    पक्ष RISCVStorePageFault:
    पक्ष RISCVInstructionPageFault:
        handleVMFaultEvent(scause);
        भङ्ग;
    स्वभाव:
        handleUserLevelFault(scause, 0);
        भङ्ग;
    }

    restore_user_context();
    UNREACHABLE();
}

शून्य VISIBLE NORETURN slowpath(syscall_t syscall)
{
    यदि (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Unknown_Syscall;
#यदि_अन्तः /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } अन्यथा {
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.is_fastpath = 0;
#यदि_अन्तः /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#यदि_परिभाषितम् CONFIG_FASTPATH
ALIGN(L1_CACHE_पङ्क्तिः_आकार)
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य VISIBLE c_handle_fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info, word_t प्रत्युत्तरम्)
#अन्यथा
शून्य VISIBLE c_handle_fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info)
#यदि_अन्तः
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, Sys_प्रत्युत्तरम्_Recv);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* DEBUG */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info, प्रत्युत्तरम्);
#अन्यथा
    fastpath_प्रत्युत्तरम्_recv(cptr, सन्देशः_Info);
#यदि_अन्तः
    UNREACHABLE();
}

ALIGN(L1_CACHE_पङ्क्तिः_आकार)
शून्य VISIBLE c_handle_fastpath_call(word_t cptr, word_t सन्देशः_Info)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, SysCall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 1;
#यदि_अन्तः /* DEBUG */

    fastpath_call(cptr, सन्देशः_Info);

    UNREACHABLE();
}
#यदि_अन्तः

शून्य VISIBLE NORETURN c_handle_syscall(word_t cptr, word_t सन्देशः_Info, syscall_t syscall)
{
    ग्रन्थिः_तालकम्_SYS;

    c_प्रविष्टिः_hook();
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_start(cptr, सन्देशः_Info, syscall);
    ks_Kernel_प्रविष्टिः.is_fastpath = 0;
#यदि_अन्तः /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
