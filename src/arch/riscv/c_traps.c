/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <arch/fastpath/fastpath.h>
#ضمن <arch/kernel/traps.h>
#ضمن <machine/debug.h>
#ضمن <api/syscall.h>
#ضمن <util.h>
#ضمن <arch/machine/hardware.h>
#ضمن <machine/fpu.h>

#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
فراغ VISIBLE NORETURN restore_user_context(فراغ)
{
    لفظ_t cur_thread_reg = (لفظ_t) گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    لفظ_t sp = read_sscratch();
    sp -= حجم(لفظ_t);
    *((لفظ_t *)sp) = cur_thread_reg;
#جے_انت

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    سیٹ_tcb_fs_state(گرہ_STATE(ksCurThread), isFpuEnable());
#جے_انت

    گرہ_UNLOCK_IF_HELD;

    asm متطاير(
        "mv t0, %[cur_thread]       \n"
        لدو_S " ra, (0*%[REGSIZE])(t0)  \n"
        لدو_S "  sp, (1*%[REGSIZE])(t0)  \n"
        لدو_S "  gp, (2*%[REGSIZE])(t0)  \n"
        لدو_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        لدو_S "  t2, (6*%[REGSIZE])(t0)  \n"
        لدو_S "  s0, (7*%[REGSIZE])(t0)  \n"
        لدو_S "  s1, (8*%[REGSIZE])(t0)  \n"
        لدو_S "  a0, (9*%[REGSIZE])(t0) \n"
        لدو_S "  a1, (10*%[REGSIZE])(t0) \n"
        لدو_S "  a2, (11*%[REGSIZE])(t0) \n"
        لدو_S "  a3, (12*%[REGSIZE])(t0) \n"
        لدو_S "  a4, (13*%[REGSIZE])(t0) \n"
        لدو_S "  a5, (14*%[REGSIZE])(t0) \n"
        لدو_S "  a6, (15*%[REGSIZE])(t0) \n"
        لدو_S "  a7, (16*%[REGSIZE])(t0) \n"
        لدو_S "  s2, (17*%[REGSIZE])(t0) \n"
        لدو_S "  s3, (18*%[REGSIZE])(t0) \n"
        لدو_S "  s4, (19*%[REGSIZE])(t0) \n"
        لدو_S "  s5, (20*%[REGSIZE])(t0) \n"
        لدو_S "  s6, (21*%[REGSIZE])(t0) \n"
        لدو_S "  s7, (22*%[REGSIZE])(t0) \n"
        لدو_S "  s8, (23*%[REGSIZE])(t0) \n"
        لدو_S "  s9, (24*%[REGSIZE])(t0) \n"
        لدو_S "  s10, (25*%[REGSIZE])(t0)\n"
        لدو_S "  s11, (26*%[REGSIZE])(t0)\n"
        لدو_S "  t3, (27*%[REGSIZE])(t0) \n"
        لدو_S "  t4, (28*%[REGSIZE])(t0) \n"
        لدو_S "  t5, (29*%[REGSIZE])(t0) \n"
        لدو_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        لدو_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#جے_بنا_تعریف ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#جے_انت
        لدو_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        لدو_S "  t1, (5*%[REGSIZE])(t0) \n"
        لدو_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(حجم(لفظ_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_رکاوٹ(فراغ)
{
    گرہ_جندرا_IRQ_IF(get_سرگرم_IRQ() != irq_remote_call_ipi);

    c_اندراج_hook();

    handle_رکاوٹ_اندراج();

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_exception(فراغ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    إذا (read_sstatus() & SSTATUS_SPP) {
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
        لکھو("\n\nKERNEL ABORT (exception within s-mode)!\n");
        لکھو("scause: 0x%"SEL4_PRIx_لفظ", stval: 0x%"SEL4_PRIx_لفظ"\n",
               read_scause(), read_stval());
#جے_انت
        halt();
    }

    گرہ_جندرا_SYS;

    c_اندراج_hook();

    لفظ_t scause = read_scause();
    بدل (scause) {
    حالة RISCVInstructionAccessFault:
    حالة RISCVLoadAccessFault:
    حالة RISCVStoreAccessFault:
    حالة RISCVLoad_ورقہ_Fault:
    حالة RISCVStore_ورقہ_Fault:
    حالة RISCVInstruction_ورقہ_Fault:
        handleVMFaultEvent(scause);
        اكسر;
    افتراضي:
        handleUserLevelFault(scause, 0);
        اكسر;
    }

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE NORETURN slowpath(syscall_t syscall)
{
    إذا (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.رستہ = اندراج_Unknown_Syscall;
#جے_انت /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#جے_انت /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#جے_تعریف_شدہ CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_ناپ)
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info, لفظ_t جواب)
#وإلا
فراغ VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info)
#جے_انت
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, Sys_جواب_Recv);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* DEBUG */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#وإلا
    fastpath_جواب_recv(cptr, پیغام_Info);
#جے_انت
    UNREACHABLE();
}

ALIGN(L1_CACHE_سطر_ناپ)
فراغ VISIBLE c_handle_fastpath_call(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#جے_انت /* DEBUG */

    fastpath_call(cptr, پیغام_Info);

    UNREACHABLE();
}
#جے_انت

فراغ VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
{
    گرہ_جندرا_SYS;

    c_اندراج_hook();
#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 0;
#جے_انت /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
