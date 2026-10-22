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
    کلمه_t cur_thread_reg = (کلمه_t) غوټه_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    کلمه_t sp = read_sscratch();
    sp -= حجم(کلمه_t);
    *((کلمه_t *)sp) = cur_thread_reg;
#د_که_پای

#که_تعریف_شوی CONFIG_HAVE_FPU
    سېټ_tcb_fs_state(غوټه_STATE(ksCurThread), isFpuEnable());
#د_که_پای

    غوټه_UNLOCK_IF_HELD;

    asm متطاير(
        "mv t0, %[cur_thread]       \n"
        بارول_S " ra, (0*%[REGSIZE])(t0)  \n"
        بارول_S "  sp, (1*%[REGSIZE])(t0)  \n"
        بارول_S "  gp, (2*%[REGSIZE])(t0)  \n"
        بارول_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        بارول_S "  t2, (6*%[REGSIZE])(t0)  \n"
        بارول_S "  s0, (7*%[REGSIZE])(t0)  \n"
        بارول_S "  s1, (8*%[REGSIZE])(t0)  \n"
        بارول_S "  a0, (9*%[REGSIZE])(t0) \n"
        بارول_S "  a1, (10*%[REGSIZE])(t0) \n"
        بارول_S "  a2, (11*%[REGSIZE])(t0) \n"
        بارول_S "  a3, (12*%[REGSIZE])(t0) \n"
        بارول_S "  a4, (13*%[REGSIZE])(t0) \n"
        بارول_S "  a5, (14*%[REGSIZE])(t0) \n"
        بارول_S "  a6, (15*%[REGSIZE])(t0) \n"
        بارول_S "  a7, (16*%[REGSIZE])(t0) \n"
        بارول_S "  s2, (17*%[REGSIZE])(t0) \n"
        بارول_S "  s3, (18*%[REGSIZE])(t0) \n"
        بارول_S "  s4, (19*%[REGSIZE])(t0) \n"
        بارول_S "  s5, (20*%[REGSIZE])(t0) \n"
        بارول_S "  s6, (21*%[REGSIZE])(t0) \n"
        بارول_S "  s7, (22*%[REGSIZE])(t0) \n"
        بارول_S "  s8, (23*%[REGSIZE])(t0) \n"
        بارول_S "  s9, (24*%[REGSIZE])(t0) \n"
        بارول_S "  s10, (25*%[REGSIZE])(t0)\n"
        بارول_S "  s11, (26*%[REGSIZE])(t0)\n"
        بارول_S "  t3, (27*%[REGSIZE])(t0) \n"
        بارول_S "  t4, (28*%[REGSIZE])(t0) \n"
        بارول_S "  t5, (29*%[REGSIZE])(t0) \n"
        بارول_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        بارول_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#که_نه_تعریف_شوی ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#د_که_پای
        بارول_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        بارول_S "  t1, (5*%[REGSIZE])(t0) \n"
        بارول_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(حجم(کلمه_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_مداخله(فراغ)
{
    غوټه_قلف_IRQ_IF(get_فعال_IRQ() != irq_remote_call_ipi);

    c_ثبت_hook();

    handle_مداخله_ثبت();

    restore_user_context();
    UNREACHABLE();
}

فراغ VISIBLE NORETURN c_handle_exception(فراغ)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    إذا (read_sstatus() & SSTATUS_SPP) {
#که_تعریف_شوی CONFIG_DEBUG_BUILD
        چاپ_کړه("\n\nKERNEL ABORT (exception within s-mode)!\n");
        چاپ_کړه("scause: 0x%"SEL4_PRIx_کلمه", stval: 0x%"SEL4_PRIx_کلمه"\n",
               read_scause(), read_stval());
#د_که_پای
        halt();
    }

    غوټه_قلف_SYS;

    c_ثبت_hook();

    کلمه_t scause = read_scause();
    بدل (scause) {
    حالة RISCVInstructionAccessFault:
    حالة RISCVLoadAccessFault:
    حالة RISCVStoreAccessFault:
    حالة RISCVLoad_پاڼه_Fault:
    حالة RISCVStore_پاڼه_Fault:
    حالة RISCVInstruction_پاڼه_Fault:
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
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.لار = ثبت_Unknown_Syscall;
#د_که_پای /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } وإلا {
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.is_fastpath = 0;
#د_که_پای /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#که_تعریف_شوی CONFIG_FASTPATH
ALIGN(L1_CACHE_کرښه_اندازه)
#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ VISIBLE c_handle_fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info, کلمه_t ځواب)
#وإلا
فراغ VISIBLE c_handle_fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info)
#د_که_پای
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, Sys_ځواب_Recv);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* DEBUG */
#که_تعریف_شوی CONFIG_KERNEL_MCS
    fastpath_ځواب_recv(cptr, پیغام_Info, ځواب);
#وإلا
    fastpath_ځواب_recv(cptr, پیغام_Info);
#د_که_پای
    UNREACHABLE();
}

ALIGN(L1_CACHE_کرښه_اندازه)
فراغ VISIBLE c_handle_fastpath_call(کلمه_t cptr, کلمه_t پیغام_Info)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, SysCall);
    ks_Kernel_ثبت.is_fastpath = 1;
#د_که_پای /* DEBUG */

    fastpath_call(cptr, پیغام_Info);

    UNREACHABLE();
}
#د_که_پای

فراغ VISIBLE NORETURN c_handle_syscall(کلمه_t cptr, کلمه_t پیغام_Info, syscall_t syscall)
{
    غوټه_قلف_SYS;

    c_ثبت_hook();
#که_تعریف_شوی TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_پیل(cptr, پیغام_Info, syscall);
    ks_Kernel_ثبت.is_fastpath = 0;
#د_که_پای /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
