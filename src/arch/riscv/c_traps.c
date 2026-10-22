/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <model/statedata.h>
#شامل <arch/fastpath/fastpath.h>
#شامل <arch/kernel/traps.h>
#شامل <machine/debug.h>
#شامل <api/syscall.h>
#شامل <util.h>
#شامل <arch/machine/hardware.h>
#شامل <machine/fpu.h>

#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
خالی VISIBLE NORETURN restore_user_context(خالی)
{
    لفظ_t cur_thread_reg = (لفظ_t) گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    لفظ_t sp = read_sscratch();
    sp -= حجم(لفظ_t);
    *((لفظ_t *)sp) = cur_thread_reg;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    سیٹ_tcb_fs_state(گرہ_STATE(ksCurThread), isFpuEnable());
#اگر_ختم

    گرہ_UNLOCK_IF_HELD;

    asm متلون(
        "mv t0, %[cur_thread]       \n"
        بھرو_S " ra, (0*%[REGSIZE])(t0)  \n"
        بھرو_S "  sp, (1*%[REGSIZE])(t0)  \n"
        بھرو_S "  gp, (2*%[REGSIZE])(t0)  \n"
        بھرو_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        بھرو_S "  t2, (6*%[REGSIZE])(t0)  \n"
        بھرو_S "  s0, (7*%[REGSIZE])(t0)  \n"
        بھرو_S "  s1, (8*%[REGSIZE])(t0)  \n"
        بھرو_S "  a0, (9*%[REGSIZE])(t0) \n"
        بھرو_S "  a1, (10*%[REGSIZE])(t0) \n"
        بھرو_S "  a2, (11*%[REGSIZE])(t0) \n"
        بھرو_S "  a3, (12*%[REGSIZE])(t0) \n"
        بھرو_S "  a4, (13*%[REGSIZE])(t0) \n"
        بھرو_S "  a5, (14*%[REGSIZE])(t0) \n"
        بھرو_S "  a6, (15*%[REGSIZE])(t0) \n"
        بھرو_S "  a7, (16*%[REGSIZE])(t0) \n"
        بھرو_S "  s2, (17*%[REGSIZE])(t0) \n"
        بھرو_S "  s3, (18*%[REGSIZE])(t0) \n"
        بھرو_S "  s4, (19*%[REGSIZE])(t0) \n"
        بھرو_S "  s5, (20*%[REGSIZE])(t0) \n"
        بھرو_S "  s6, (21*%[REGSIZE])(t0) \n"
        بھرو_S "  s7, (22*%[REGSIZE])(t0) \n"
        بھرو_S "  s8, (23*%[REGSIZE])(t0) \n"
        بھرو_S "  s9, (24*%[REGSIZE])(t0) \n"
        بھرو_S "  s10, (25*%[REGSIZE])(t0)\n"
        بھرو_S "  s11, (26*%[REGSIZE])(t0)\n"
        بھرو_S "  t3, (27*%[REGSIZE])(t0) \n"
        بھرو_S "  t4, (28*%[REGSIZE])(t0) \n"
        بھرو_S "  t5, (29*%[REGSIZE])(t0) \n"
        بھرو_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        بھرو_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#اگر_غیر_تعریف_شدہ ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#اگر_ختم
        بھرو_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        بھرو_S "  t1, (5*%[REGSIZE])(t0) \n"
        بھرو_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(حجم(لفظ_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

خالی VISIBLE NORETURN c_handle_مداخلت(خالی)
{
    گرہ_تالا_IRQ_IF(get_فعال_IRQ() != irq_remote_call_ipi);

    c_اندراج_hook();

    handle_مداخلت_اندراج();

    restore_user_context();
    UNREACHABLE();
}

خالی VISIBLE NORETURN c_handle_exception(خالی)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    اگر (read_sstatus() & SSTATUS_SPP) {
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
        لکھو("\n\nKERNEL ABORT (exception within s-mode)!\n");
        لکھو("scause: 0x%"SEL4_PRIx_لفظ", stval: 0x%"SEL4_PRIx_لفظ"\n",
               read_scause(), read_stval());
#اگر_ختم
        halt();
    }

    گرہ_تالا_SYS;

    c_اندراج_hook();

    لفظ_t scause = read_scause();
    انتخاب (scause) {
    حالت RISCVInstructionAccessFault:
    حالت RISCVLoadAccessFault:
    حالت RISCVStoreAccessFault:
    حالت RISCVLoad_صفحہ_Fault:
    حالت RISCVStore_صفحہ_Fault:
    حالت RISCVInstruction_صفحہ_Fault:
        handleVMFaultEvent(scause);
        توڑو;
    طےشدہ:
        handleUserLevelFault(scause, 0);
        توڑو;
    }

    restore_user_context();
    UNREACHABLE();
}

خالی VISIBLE NORETURN slowpath(syscall_t syscall)
{
    اگر (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.راستہ = اندراج_Unknown_Syscall;
#اگر_ختم /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } ورنہ {
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = 0;
#اگر_ختم /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#اگر_تعریف_شدہ CONFIG_FASTPATH
ALIGN(L1_CACHE_سطر_سائز)
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info, لفظ_t جواب)
#ورنہ
خالی VISIBLE c_handle_fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info)
#اگر_ختم
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, Sys_جواب_Recv);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* DEBUG */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    fastpath_جواب_recv(cptr, پیغام_Info, جواب);
#ورنہ
    fastpath_جواب_recv(cptr, پیغام_Info);
#اگر_ختم
    UNREACHABLE();
}

ALIGN(L1_CACHE_سطر_سائز)
خالی VISIBLE c_handle_fastpath_call(لفظ_t cptr, لفظ_t پیغام_Info)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, SysCall);
    ks_Kernel_اندراج.is_fastpath = 1;
#اگر_ختم /* DEBUG */

    fastpath_call(cptr, پیغام_Info);

    UNREACHABLE();
}
#اگر_ختم

خالی VISIBLE NORETURN c_handle_syscall(لفظ_t cptr, لفظ_t پیغام_Info, syscall_t syscall)
{
    گرہ_تالا_SYS;

    c_اندراج_hook();
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_آغاز(cptr, پیغام_Info, syscall);
    ks_Kernel_اندراج.is_fastpath = 0;
#اگر_ختم /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
