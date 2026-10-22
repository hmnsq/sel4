/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/fastpath/fastpath.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <machine/debug.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <arch/machine/hardware.h>
#అన్తర్భావ <machine/fpu.h>

#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
శూన్య VISIBLE NORETURN restore_user_context(శూన్య)
{
    మాట_t cur_thread_reg = (మాట_t) కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers;
    c_exit_hook();

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    మాట_t sp = read_sscratch();
    sp -= పరిమాణం(మాట_t);
    *((మాట_t *)sp) = cur_thread_reg;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HAVE_FPU
    సమితి_tcb_fs_state(కణుపు_STATE(ksCurThread), isFpuEnable());
#అయితే_ముగింపు

    కణుపు_UNLOCK_IF_HELD;

    asm అస్థిర(
        "mv t0, %[cur_thread]       \n"
        ఎక్కించు_S " ra, (0*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  sp, (1*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  gp, (2*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  tp, (3*%[REGSIZE])(t0)  \n"
        /* skip t0/x5, t1/x6, they are restored later */
        /* no-op store conditional to clear monitor state */
        /* this may succeed in implementations with very large reservations, but the saved ra is dead */
        "sc.w zero, zero, (t0)\n"
        ఎక్కించు_S "  t2, (6*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  s0, (7*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  s1, (8*%[REGSIZE])(t0)  \n"
        ఎక్కించు_S "  a0, (9*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a1, (10*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a2, (11*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a3, (12*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a4, (13*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a5, (14*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a6, (15*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  a7, (16*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s2, (17*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s3, (18*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s4, (19*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s5, (20*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s6, (21*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s7, (22*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s8, (23*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s9, (24*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  s10, (25*%[REGSIZE])(t0)\n"
        ఎక్కించు_S "  s11, (26*%[REGSIZE])(t0)\n"
        ఎక్కించు_S "  t3, (27*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  t4, (28*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  t5, (29*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  t6, (30*%[REGSIZE])(t0) \n"
        /* get sepc */
        ఎక్కించు_S "  t1, (34*%[REGSIZE])(t0)\n"
        "csrw sepc, t1  \n"
#నిర్వచితం_కాకపోతే ENABLE_SMP_SUPPORT
        /* Write back sscratch with cur_thread_reg to get it back on the next trap entry */
        "csrw sscratch, t0         \n"
#అయితే_ముగింపు
        ఎక్కించు_S "  t1, (32*%[REGSIZE])(t0) \n"
        "csrw sstatus, t1\n"

        ఎక్కించు_S "  t1, (5*%[REGSIZE])(t0) \n"
        ఎక్కించు_S "  t0, (4*%[REGSIZE])(t0) \n"
        "sret"
        : /* no output */
        : [REGSIZE] "i"(పరిమాణం(మాట_t)),
        [cur_thread] "r"(cur_thread_reg)
        : "memory"
    );

    UNREACHABLE();
}

శూన్య VISIBLE NORETURN c_handle_అంతరాయం(శూన్య)
{
    కణుపు_తాళం_IRQ_IF(get_చురుకైన_IRQ() != irq_remote_call_ipi);

    c_నమోదు_hook();

    handle_అంతరాయం_నమోదు();

    restore_user_context();
    UNREACHABLE();
}

శూన్య VISIBLE NORETURN c_handle_exception(శూన్య)
{
    /* Any exception from S-mode is a kernel fault, not a user fault.
     * Kernel faults should not exist, so print potential debug info and halt. */
    ఒకవేళ (read_sstatus() & SSTATUS_SPP) {
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
        రాయి("\n\nKERNEL ABORT (exception within s-mode)!\n");
        రాయి("scause: 0x%"SEL4_PRIx_మాట", stval: 0x%"SEL4_PRIx_మాట"\n",
               read_scause(), read_stval());
#అయితే_ముగింపు
        halt();
    }

    కణుపు_తాళం_SYS;

    c_నమోదు_hook();

    మాట_t scause = read_scause();
    ఎంపిక (scause) {
    స్థితి RISCVInstructionAccessFault:
    స్థితి RISCVLoadAccessFault:
    స్థితి RISCVStoreAccessFault:
    స్థితి RISCVLoad_పుట_Fault:
    స్థితి RISCVStore_పుట_Fault:
    స్థితి RISCVInstruction_పుట_Fault:
        handleVMFaultEvent(scause);
        ఆపు;
    అప్రమేయ:
        handleUserLevelFault(scause, 0);
        ఆపు;
    }

    restore_user_context();
    UNREACHABLE();
}

శూన్య VISIBLE NORETURN slowpath(syscall_t syscall)
{
    ఒకవేళ (unlikely(syscall < SYSCALL_MIN || syscall > SYSCALL_MAX)) {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.మార్గం = నమోదు_Unknown_Syscall;
#అయితే_ముగింపు /* TRACK_KERNEL_ENTRIES */
        /* Contrary to the name, this handles all non-standard syscalls used in
         * debug builds also.
         */
        handleUnknownSyscall(syscall);
    } లేకపోతే {
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.is_fastpath = 0;
#అయితే_ముగింపు /* TRACK KERNEL ENTRIES */
        handleSyscall(syscall);
    }

    restore_user_context();
    UNREACHABLE();
}

#నిర్వచితమైతే CONFIG_FASTPATH
ALIGN(L1_CACHE_పంక్తి_కొలత)
#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య VISIBLE c_handle_fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info, మాట_t జవాబు)
#లేకపోతే
శూన్య VISIBLE c_handle_fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info)
#అయితే_ముగింపు
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, Sys_జవాబు_Recv);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* DEBUG */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    fastpath_జవాబు_recv(cptr, సందేశం_Info, జవాబు);
#లేకపోతే
    fastpath_జవాబు_recv(cptr, సందేశం_Info);
#అయితే_ముగింపు
    UNREACHABLE();
}

ALIGN(L1_CACHE_పంక్తి_కొలత)
శూన్య VISIBLE c_handle_fastpath_call(మాట_t cptr, మాట_t సందేశం_Info)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, SysCall);
    ks_Kernel_నమోదు.is_fastpath = 1;
#అయితే_ముగింపు /* DEBUG */

    fastpath_call(cptr, సందేశం_Info);

    UNREACHABLE();
}
#అయితే_ముగింపు

శూన్య VISIBLE NORETURN c_handle_syscall(మాట_t cptr, మాట_t సందేశం_Info, syscall_t syscall)
{
    కణుపు_తాళం_SYS;

    c_నమోదు_hook();
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    benchmark_debug_syscall_ప్రారంభం(cptr, సందేశం_Info, syscall);
    ks_Kernel_నమోదు.is_fastpath = 0;
#అయితే_ముగింపు /* DEBUG */
    slowpath(syscall);

    UNREACHABLE();
}
