/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

#అన్తర్భావ <mode/smp/ipi.h>
#అన్తర్భావ <smp/ipi.h>
#అన్తర్భావ <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
శూన్య ipiStallCoreCallback(bool_t irq_మార్గం)
{
    మాట_t cpu = getCurrentCPUIndex();
    clh_కణుపు_t *కణుపు = &big_kernel_తాళం.కణుపు[cpu];

    ఒకవేళ (clh_is_self_లోపల_వరుస() && !irq_మార్గం) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        ఒకవేళ (thread_state_ptr_get_tsType(&కణుపు_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_వరుసలో_చేర్చు_CURRENT_TCB;
        switch_గమ్యం_Idle_Thread();
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        commit_సమయం();
        కణుపు_STATE(ksCurSC) = కణుపు_STATE(ksIdleThread)->tcbSchedContext;
#అయితే_ముగింపు
        కణుపు_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        కణుపు->ipi = 0;
#నిర్వచితమైతే CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#అయితే_ముగింపు
        ipi_wait();

        /* Continue waiting on lock */
        అయితే (కణుపు->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            ఒకవేళ (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                ప్రతిజ్ఞా(big_kernel_తాళం.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_గమ్యం_IRQT(cpu, irq_remote_call_ipi), irq_మార్గం);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm అస్థిర("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } లేకపోతే {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_వరుసలో_చేర్చు_CURRENT_TCB;
        switch_గమ్యం_Idle_Thread();
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        commit_సమయం();
        కణుపు_STATE(ksCurSC) = కణుపు_STATE(ksIdleThread)->tcbSchedContext;
#అయితే_ముగింపు
        కణుపు_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

శూన్య ipi_wait(శూన్య)
{
    ipi_state_t *ipi = &big_kernel_తాళం.ipi;
    మాట_t cores = ipi->మొత్తం_Core_Barrier;
    మాట_t localsense = ipi->globalsense;
    మాట_t *లెక్క = &ipi->లెక్క;

    ఒకవేళ (__atomic_fetch_add(లెక్క, 1, __ATOMIC_ACQ_REL) == cores) {
        *లెక్క = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    అయితే (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

స్థిర అంతఃవరుస శూన్య init_ipi_వాదనలు(IpiRemoteCall_t func,
                                 మాట_t data1, మాట_t data2, మాట_t data3,
                                 మాట_t ముసుగు_2)
{
    ipi_state_t *ipi = &big_kernel_తాళం.ipi;

    ipi->remoteCall = func;
    ipi->వాదనలు[0] = data1;
    ipi->వాదనలు[1] = data2;
    ipi->వాదనలు[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->మొత్తం_Core_Barrier = popcountl(ముసుగు_2);
}

శూన్య handleIPI(irq_t irq, bool_t irq_మార్గం)
{
    ipi_state_t *ipi = &big_kernel_తాళం.ipi;

    ఒకవేళ (IRQT_గమ్యం_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->వాదనలు[0], ipi->వాదనలు[1], ipi->వాదనలు[2], irq_మార్గం);
    } లేకపోతే ఒకవేళ (IRQT_గమ్యం_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#నిర్వచితమైతే CONFIG_ARCH_RISCV
        ifence_local();
#అయితే_ముగింపు
    } లేకపోతే {
        fail("Invalid IPI");
    }
}

శూన్య do_Remote_ముసుగు_Op(IpiRemoteCall_t func, మాట_t data1, మాట_t data2, మాట_t data3, మాట_t ముసుగు_2)
{
    /* make sure the current core is not set in the mask */
    ముసుగు_2 &= ~బిట్టు(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    ఒకవేళ (ముసుగు_2 != 0) {
        init_ipi_వాదనలు(func, data1, data2, data3, ముసుగు_2);

        /* make sure no resource access passes from this point */
        asm అస్థిర("" ::: "memory");
        ipi_send_ముసుగు(CORE_IRQ_గమ్యం_IRQT(0, irq_remote_call_ipi), ముసుగు_2, true);
        ipi_wait();
    }
}

శూన్య do_ముసుగు_Reschedule(మాట_t ముసుగు_2)
{
    /* make sure the current core is not set in the mask */
    ముసుగు_2 &= ~బిట్టు(getCurrentCPUIndex());
    ఒకవేళ (ముసుగు_2 != 0) {
        ipi_send_ముసుగు(CORE_IRQ_గమ్యం_IRQT(0, irq_reschedule_ipi), ముసుగు_2, false);
    }
}

శూన్య generic_ipi_send_ముసుగు(irq_t ipi, మాట_t ముసుగు_2, bool_t isBlocking)
{
    మాట_t nr_లక్ష్యం_cores = 0;
    uint16_t లక్ష్యం_cores[CONFIG_MAX_అంకె_NODES];

    అయితే (ముసుగు_2) {
        పూర్ణాంకం సూచీ = మాట_బిట్లు - 1 - clzl(ముసుగు_2);
        ఒకవేళ (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_తాళం.కణుపు[సూచీ].ipi, 1, __ATOMIC_RELEASE);
            లక్ష్యం_cores[nr_లక్ష్యం_cores] = సూచీ;
            nr_లక్ష్యం_cores++;
        } లేకపోతే {
            IPI_MEM_BARRIER;
            ipi_send_లక్ష్యం(ipi, cpu_సూచీ_గమ్యం_ID(సూచీ));
        }
        ముసుగు_2 &= ~బిట్టు(సూచీ);
    }

    ఒకవేళ (nr_లక్ష్యం_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        కొరకు (పూర్ణాంకం i = 0; i < nr_లక్ష్యం_cores; i++) {
            ipi_send_లక్ష్యం(ipi, cpu_సూచీ_గమ్యం_ID(లక్ష్యం_cores[i]));
        }
    }
}

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(శూన్య)
{
#నిర్వచితమైతే CONFIG_ARCH_ARM
    మాట_t లక్ష్యం = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), cap_రిజిస్టరు);
    మాట_t irq = get_రిజిస్టరు(కణుపు_STATE(ksCurThread), సందేశం_Info_రిజిస్టరు);
    ఒకవేళ (లక్ష్యం > CONFIG_MAX_అంకె_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    ఒకవేళ (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_లక్ష్యం(CORE_IRQ_గమ్యం_IRQT(0, irq), బిట్టు(లక్ష్యం));
    తిరిగి EXCEPTION_NONE;
#లేకపోతే /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#అయితే_ముగింపు  /* [not] CONFIG_ARCH_ARM */
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
