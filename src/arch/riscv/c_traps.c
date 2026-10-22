/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ VISIBLE NORETURN restore_user_context(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t cur_thread_reg = (ಶಬ್ದ_t) ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಶಬ್ದ_t sp = read_sscratch();
    sp -= ಗಾತ್ರ(ಶಬ್ದ_t);
    *((ಶಬ್ದ_t *)sp) = cur_thread_reg;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    ಗಣ_tcb_fs_state(ಗಂಟು_STATE(ksCurThread), isFpuEnable());
#ಆದರೆ_ಅಂತ್ಯ

    ಗಂಟು_UNLOCK_IF_HELD;

    asm ಅಸ್ಥಿರ(
        "mv t0, %[cur_thread]       \n"
        ತುಂಬು_S " ra, (0*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  sp, (1*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  gp, (2*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        ತುಂಬು_S "  t2, (6*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  s0, (7*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  s1, (8*%[REGSIZE])(t0)  \n"
        ತುಂಬು_S "  a0, (9*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a1, (10*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a2, (11*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a3, (12*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a4, (13*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a5, (14*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a6, (15*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  a7, (16*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s2, (17*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s3, (18*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s4, (19*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s5, (20*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s6, (21*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s7, (22*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s8, (23*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s9, (24*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  s10, (25*%[REGSIZE])(t0)\n"
        ತುಂಬು_S "  s11, (26*%[REGSIZE])(t0)\n"
        ತುಂಬು_S "  t3, (27*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  t4, (28*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  t5, (29*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        ತುಂಬು_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#ಆದರೆ_ಅಂತ್ಯ
        ತುಂಬು_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        ತುಂಬು_S "  t1, (5*%[REGSIZE])(t0) \n"
        ತುಂಬು_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(ಗಾತ್ರ(ಶಬ್ದ_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    ಗಂಟು_ಬೀಗ_IRQ_IF(get_ಸಕ್ರಿಯ_IRQ() != irq_remote_call_ipi);

    c_ನಮೂದು_hook();

    handle_ಅಡಚಣೆ_ನಮೂದು();

    restore_user_context();
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN c_handle_exception(ಶೂನ್ಯ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    ಆದರೆ (read_sstatus() & SSTATUS_SPP) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
        ಬರೆ("\n\nKERNEL ABORT (exception within s-mode)!\n");
        ಬರೆ("scause: 0x%"SEL4_PRIx_ಶಬ್ದ", stval: 0x%"SEL4_PRIx_ಶಬ್ದ"\n",
               read_scause(), read_stval());
#ಆದರೆ_ಅಂತ್ಯ
        halt();
    }

    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();

    ಶಬ್ದ_t scause = read_scause();
    ಆಯ್ಕೆ (scause) {
    ಸ್ಥಿತಿ RISCVInstructionAccessFault:
    ಸ್ಥಿತಿ RISCVLoadAccessFault:
    ಸ್ಥಿತಿ RISCVStoreAccessFault:
    ಸ್ಥಿತಿ RISCVLoad_ಪುಟ_Fault:
    ಸ್ಥಿತಿ RISCVStore_ಪುಟ_Fault:
    ಸ್ಥಿತಿ RISCVInstruction_ಪುಟ_Fault:
        handleVMFaultEvent(scause);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        handleUserLevelFault(scause, 0);
        ನಿಲ್ಲಿಸು;
    }

    restore_user_context();
    UNREACHABLE();
}

ಶೂನ್ಯ VISIBLE NORETURN slowpath(syscall_t syscall)
{
    ಆದರೆ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Unknown_Syscall;
#ಆದರೆ_ಅಂತ್ಯ /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.is_fastpath = 0;
#ಆದರೆ_ಅಂತ್ಯ /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_FASTPATH
ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ VISIBLE c_handle_fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, ಶಬ್ದ_t ಉತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ VISIBLE c_handle_fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, Sys_ಉತ್ತರ_Recv);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info, ಉತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
    fastpath_ಉತ್ತರ_recv(cptr, ಸಂದೇಶ_Info);
#ಆದರೆ_ಅಂತ್ಯ
    UNREACHABLE();
}

ALIGN(L1_CACHE_ಸಾಲು_ಗಾತ್ರ)
ಶೂನ್ಯ VISIBLE c_handle_fastpath_call(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, SysCall);
    ks_Kernel_ನಮೂದು.is_fastpath = 1;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

    fastpath_call(cptr, ಸಂದೇಶ_Info);

    UNREACHABLE();
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ VISIBLE NORETURN c_handle_syscall(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, syscall_t syscall)
{
    ಗಂಟು_ಬೀಗ_SYS;

    c_ನಮೂದು_hook();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ಆರಂಭ(cptr, ಸಂದೇಶ_Info, syscall);
    ks_Kernel_ನಮೂದು.is_fastpath = 0;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
