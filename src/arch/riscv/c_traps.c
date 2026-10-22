/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/fastpath/fastpath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ਖਾਲੀ VISIBLE NORETURN restore_user_context(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t cur_thread_reg = (ਸ਼ਬਦ_t) ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਸ਼ਬਦ_t sp = read_sscratch();
    sp -= ਆਕਾਰ(ਸ਼ਬਦ_t);
    *((ਸ਼ਬਦ_t *)sp) = cur_thread_reg;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    ਸੈੱਟ_tcb_fs_state(ਗੰਢ_STATE(ksCurThread), isFpuEnable());
#ਜੇ_ਅੰਤ

    ਗੰਢ_UNLOCK_IF_HELD;

    asm ਅਸਥਿਰ(
        "mv t0, %[cur_thread]       \n"
        ਲੱਦੋ_S " ra, (0*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  sp, (1*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  gp, (2*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        ਲੱਦੋ_S "  t2, (6*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  s0, (7*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  s1, (8*%[REGSIZE])(t0)  \n"
        ਲੱਦੋ_S "  a0, (9*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a1, (10*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a2, (11*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a3, (12*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a4, (13*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a5, (14*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a6, (15*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  a7, (16*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s2, (17*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s3, (18*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s4, (19*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s5, (20*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s6, (21*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s7, (22*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s8, (23*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s9, (24*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  s10, (25*%[REGSIZE])(t0)\n"
        ਲੱਦੋ_S "  s11, (26*%[REGSIZE])(t0)\n"
        ਲੱਦੋ_S "  t3, (27*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  t4, (28*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  t5, (29*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        ਲੱਦੋ_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#ਜੇ_ਅੰਤ
        ਲੱਦੋ_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        ਲੱਦੋ_S "  t1, (5*%[REGSIZE])(t0) \n"
        ਲੱਦੋ_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(ਆਕਾਰ(ਸ਼ਬਦ_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

ਖਾਲੀ VISIBLE NORETURN c_handle_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    ਗੰਢ_ਤਾਲਾ_IRQ_IF(get_ਸਰਗਰਮ_IRQ() != irq_remote_call_ipi);

    c_ਇੰਦਰਾਜ_hook();

    handle_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ();

    restore_user_context();
    UNREACHABLE();
}

ਖਾਲੀ VISIBLE NORETURN c_handle_exception(ਖਾਲੀ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    ਜੇ (read_sstatus() & SSTATUS_SPP) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
        ਲਿਖੋ("\n\nKERNEL ABORT (exception within s-mode)!\n");
        ਲਿਖੋ("scause: 0x%"SEL4_PRIx_ਸ਼ਬਦ", stval: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n",
               read_scause(), read_stval());
#ਜੇ_ਅੰਤ
        halt();
    }

    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();

    ਸ਼ਬਦ_t scause = read_scause();
    ਚੋਣ (scause) {
    ਹਾਲਤ RISCVInstructionAccessFault:
    ਹਾਲਤ RISCVLoadAccessFault:
    ਹਾਲਤ RISCVStoreAccessFault:
    ਹਾਲਤ RISCVLoad_ਪੰਨਾ_Fault:
    ਹਾਲਤ RISCVStore_ਪੰਨਾ_Fault:
    ਹਾਲਤ RISCVInstruction_ਪੰਨਾ_Fault:
        handleVMFaultEvent(scause);
        ਰੋਕੋ;
    ਮੂਲ:
        handleUserLevelFault(scause, 0);
        ਰੋਕੋ;
    }

    restore_user_context();
    UNREACHABLE();
}

ਖਾਲੀ VISIBLE NORETURN slowpath(syscall_t syscall)
{
    ਜੇ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Unknown_Syscall;
#ਜੇ_ਅੰਤ /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
        ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 0;
#ਜੇ_ਅੰਤ /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_FASTPATH
ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ VISIBLE c_handle_fastpath_ਜਵਾਬ_recv(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, ਸ਼ਬਦ_t ਜਵਾਬ)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ VISIBLE c_handle_fastpath_ਜਵਾਬ_recv(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info)
#ਜੇ_ਅੰਤ
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, Sys_ਜਵਾਬ_Recv);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* DEBUG */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info, ਜਵਾਬ);
#ਨਹੀਂਤਾਂ
    fastpath_ਜਵਾਬ_recv(cptr, ਸੁਨੇਹਾ_Info);
#ਜੇ_ਅੰਤ
    UNREACHABLE();
}

ALIGN(L1_CACHE_ਸਤਰ_ਮਾਪ)
ਖਾਲੀ VISIBLE c_handle_fastpath_call(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, SysCall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 1;
#ਜੇ_ਅੰਤ /* DEBUG */

    fastpath_call(cptr, ਸੁਨੇਹਾ_Info);

    UNREACHABLE();
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ VISIBLE NORETURN c_handle_syscall(ਸ਼ਬਦ_t cptr, ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Info, syscall_t syscall)
{
    ਗੰਢ_ਤਾਲਾ_SYS;

    c_ਇੰਦਰਾਜ_hook();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ਸ਼ੁਰੂ(cptr, ਸੁਨੇਹਾ_Info, syscall);
    ks_Kernel_ਇੰਦਰਾਜ.is_fastpath = 0;
#ਜੇ_ਅੰਤ /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
