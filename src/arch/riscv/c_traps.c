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
    کلمه_t cur_thread_reg = (کلمه_t) گره_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    کلمه_t sp = read_sscratch();
    sp -= حجم(کلمه_t);
    *((کلمه_t *)sp) = cur_thread_reg;
#پایان_اگر

#اگر_تعریف_شده CONFIG_HAVE_FPU
    مجموعه_tcb_fs_state(گره_STATE(ksCurThread), isFpuEnable());
#پایان_اگر

    گره_UNLOCK_IF_HELD;

    asm متطاير(
        "mv t0, %[cur_thread]       \n"
        بارگیری_S " ra, (0*%[REGSIZE])(t0)  \n"
        بارگیری_S "  sp, (1*%[REGSIZE])(t0)  \n"
        بارگیری_S "  gp, (2*%[REGSIZE])(t0)  \n"
        بارگیری_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        بارگیری_S "  t2, (6*%[REGSIZE])(t0)  \n"
        بارگیری_S "  s0, (7*%[REGSIZE])(t0)  \n"
        بارگیری_S "  s1, (8*%[REGSIZE])(t0)  \n"
        بارگیری_S "  a0, (9*%[REGSIZE])(t0) \n"
        بارگیری_S "  a1, (10*%[REGSIZE])(t0) \n"
        بارگیری_S "  a2, (11*%[REGSIZE])(t0) \n"
        بارگیری_S "  a3, (12*%[REGSIZE])(t0) \n"
        بارگیری_S "  a4, (13*%[REGSIZE])(t0) \n"
        بارگیری_S "  a5, (14*%[REGSIZE])(t0) \n"
        بارگیری_S "  a6, (15*%[REGSIZE])(t0) \n"
        بارگیری_S "  a7, (16*%[REGSIZE])(t0) \n"
        بارگیری_S "  s2, (17*%[REGSIZE])(t0) \n"
        بارگیری_S "  s3, (18*%[REGSIZE])(t0) \n"
        بارگیری_S "  s4, (19*%[REGSIZE])(t0) \n"
        بارگیری_S "  s5, (20*%[REGSIZE])(t0) \n"
        بارگیری_S "  s6, (21*%[REGSIZE])(t0) \n"
        بارگیری_S "  s7, (22*%[REGSIZE])(t0) \n"
        بارگیری_S "  s8, (23*%[REGSIZE])(t0) \n"
        بارگیری_S "  s9, (24*%[REGSIZE])(t0) \n"
        بارگیری_S "  s10, (25*%[REGSIZE])(t0)\n"
        بارگیری_S "  s11, (26*%[REGSIZE])(t0)\n"
        بارگیری_S "  t3, (27*%[REGSIZE])(t0) \n"
        بارگیری_S "  t4, (28*%[REGSIZE])(t0) \n"
        بارگیری_S "  t5, (29*%[REGSIZE])(t0) \n"
        بارگیری_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        بارگیری_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#اگر_تعریف_نشده ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#پایان_اگر
        بارگیری_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        بارگیری_S "  t1, (5*%[REGSIZE])(t0) \n"
        بارگیری_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(حجم(کلمه_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_وقفه(فراغ)
{
    گره_قفل_IRQ_IF(get_فعال_IRQ() != irq_remote_call_ipi);

    c_مدخل_hook();

    handle_وقفه_مدخل();

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_exception(فراغ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    إذا (read_sstatus() & SSTATUS_SPP) {
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
        چاپ_کن("\n\nKERNEL ABORT (exception within s-mode)!\n");
        چاپ_کن("scause: 0x%"SEL4_PRIx_کلمه", stval: 0x%"SEL4_PRIx_کلمه"\n",
               read_scause(), read_stval());
#پایان_اگر
        halt();
    }

    گره_قفل_SYS;

    c_مدخل_hook();

    کلمه_t scause = read_scause();
    بدل (scause) {
    حالة RISCVInstructionAccessFault:
    حالة RISCVLoadAccessFault:
    حالة RISCVStoreAccessFault:
    حالة RISCVLoad_صفحه_Fault:
    حالة RISCVStore_صفحه_Fault:
    حالة RISCVInstruction_صفحه_Fault:
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
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.مسیر = مدخل_Unknown_Syscall;
#پایان_اگر /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
        ks_Kernel_مدخل.is_fastpath = 0;
#پایان_اگر /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شده CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_اندازه)
#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_پاسخ_recv(کلمه_t cptr, کلمه_t پیام_Info, کلمه_t پاسخ)
#وإلا
فراغ VISIBLE c_handle_fastpath_پاسخ_recv(کلمه_t cptr, کلمه_t پیام_Info)
#پایان_اگر
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, Sys_پاسخ_Recv);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* DEBUG */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    fastpath_پاسخ_recv(cptr, پیام_Info, پاسخ);
#وإلا
    fastpath_پاسخ_recv(cptr, پیام_Info);
#پایان_اگر
    UNREACHABLE();
}

ALIGN(L1_CACHE_سطر_اندازه)
فراغ VISIBLE c_handle_fastpath_call(کلمه_t cptr, کلمه_t پیام_Info)
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, SysCall);
    ks_Kernel_مدخل.is_fastpath = 1;
#پایان_اگر /* DEBUG */

    fastpath_call(cptr, پیام_Info);

    UNREACHABLE();
}
#پایان_اگر

فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیام_Info, syscall_t syscall)
{
    گره_قفل_SYS;

    c_مدخل_hook();
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_شروع(cptr, پیام_Info, syscall);
    ks_Kernel_مدخل.is_fastpath = 0;
#پایان_اگر /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
