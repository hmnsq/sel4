/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

#ਅਨ੍ਤਰ੍ਭਾਵ <mode/smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
ਖਾਲੀ ipiStallCoreCallback(bool_t irq_ਰਸਤਾ)
{
    ਸ਼ਬਦ_t cpu = getCurrentCPUIndex();
    clh_ਗੰਢ_t *ਗੰਢ = &big_kernel_ਤਾਲਾ.ਗੰਢ[cpu];

    ਜੇ (clh_is_self_ਵਿੱਚ_ਕਤਾਰ() && !irq_ਰਸਤਾ) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        ਜੇ (thread_state_ptr_get_tsType(&ਗੰਢ_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_CURRENT_TCB;
        switch_ਮੰਜ਼ਿਲ_Idle_Thread();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        commit_ਸਮਾਂ();
        ਗੰਢ_STATE(ksCurSC) = ਗੰਢ_STATE(ksIdleThread)->tcbSchedContext;
#ਜੇ_ਅੰਤ
        ਗੰਢ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        ਗੰਢ->ipi = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#ਜੇ_ਅੰਤ
        ipi_wait();

        /* Continue waiting on lock */
        ਜਦੋਂ (ਗੰਢ->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            ਜੇ (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                ਪ੍ਰਤਿਜ੍ਞਾ(big_kernel_ਤਾਲਾ.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(cpu, irq_remote_call_ipi), irq_ਰਸਤਾ);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm ਅਸਥਿਰ("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } ਨਹੀਂਤਾਂ {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_CURRENT_TCB;
        switch_ਮੰਜ਼ਿਲ_Idle_Thread();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        commit_ਸਮਾਂ();
        ਗੰਢ_STATE(ksCurSC) = ਗੰਢ_STATE(ksIdleThread)->tcbSchedContext;
#ਜੇ_ਅੰਤ
        ਗੰਢ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

ਖਾਲੀ ipi_wait(ਖਾਲੀ)
{
    ipi_state_t *ipi = &big_kernel_ਤਾਲਾ.ipi;
    ਸ਼ਬਦ_t cores = ipi->ਕੁੱਲ_Core_Barrier;
    ਸ਼ਬਦ_t localsense = ipi->globalsense;
    ਸ਼ਬਦ_t *ਗਿਣਤੀ_2 = &ipi->ਗਿਣਤੀ_2;

    ਜੇ (__atomic_fetch_add(ਗਿਣਤੀ_2, 1, __ATOMIC_ACQ_REL) == cores) {
        *ਗਿਣਤੀ_2 = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    ਜਦੋਂ (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ init_ipi_ਦਲੀਲਾਂ(IpiRemoteCall_t func,
                                 ਸ਼ਬਦ_t data1, ਸ਼ਬਦ_t data2, ਸ਼ਬਦ_t data3,
                                 ਸ਼ਬਦ_t ਨਕਾਬ_2)
{
    ipi_state_t *ipi = &big_kernel_ਤਾਲਾ.ipi;

    ipi->remoteCall = func;
    ipi->ਦਲੀਲਾਂ[0] = data1;
    ipi->ਦਲੀਲਾਂ[1] = data2;
    ipi->ਦਲੀਲਾਂ[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->ਕੁੱਲ_Core_Barrier = popcountl(ਨਕਾਬ_2);
}

ਖਾਲੀ handleIPI(irq_t irq, bool_t irq_ਰਸਤਾ)
{
    ipi_state_t *ipi = &big_kernel_ਤਾਲਾ.ipi;

    ਜੇ (IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->ਦਲੀਲਾਂ[0], ipi->ਦਲੀਲਾਂ[1], ipi->ਦਲੀਲਾਂ[2], irq_ਰਸਤਾ);
    } ਨਹੀਂਤਾਂ ਜੇ (IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_RISCV
        ifence_local();
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ {
        fail("Invalid IPI");
    }
}

ਖਾਲੀ do_Remote_ਨਕਾਬ_Op(IpiRemoteCall_t func, ਸ਼ਬਦ_t data1, ਸ਼ਬਦ_t data2, ਸ਼ਬਦ_t data3, ਸ਼ਬਦ_t ਨਕਾਬ_2)
{
    /* make sure the current core is not set in the mask */
    ਨਕਾਬ_2 &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    ਜੇ (ਨਕਾਬ_2 != 0) {
        init_ipi_ਦਲੀਲਾਂ(func, data1, data2, data3, ਨਕਾਬ_2);

        /* make sure no resource access passes from this point */
        asm ਅਸਥਿਰ("" ::: "memory");
        ipi_send_ਨਕਾਬ(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, irq_remote_call_ipi), ਨਕਾਬ_2, true);
        ipi_wait();
    }
}

ਖਾਲੀ do_ਨਕਾਬ_Reschedule(ਸ਼ਬਦ_t ਨਕਾਬ_2)
{
    /* make sure the current core is not set in the mask */
    ਨਕਾਬ_2 &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(getCurrentCPUIndex());
    ਜੇ (ਨਕਾਬ_2 != 0) {
        ipi_send_ਨਕਾਬ(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, irq_reschedule_ipi), ਨਕਾਬ_2, false);
    }
}

ਖਾਲੀ generic_ipi_send_ਨਕਾਬ(irq_t ipi, ਸ਼ਬਦ_t ਨਕਾਬ_2, bool_t isBlocking)
{
    ਸ਼ਬਦ_t nr_ਨਿਸ਼ਾਨਾ_cores = 0;
    uint16_t ਨਿਸ਼ਾਨਾ_cores[CONFIG_MAX_ਅੰਕ_NODES];

    ਜਦੋਂ (ਨਕਾਬ_2) {
        ਪੂਰਨ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(ਨਕਾਬ_2);
        ਜੇ (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_ਤਾਲਾ.ਗੰਢ[ਸੂਚਕਾਂਕ].ipi, 1, __ATOMIC_RELEASE);
            ਨਿਸ਼ਾਨਾ_cores[nr_ਨਿਸ਼ਾਨਾ_cores] = ਸੂਚਕਾਂਕ;
            nr_ਨਿਸ਼ਾਨਾ_cores++;
        } ਨਹੀਂਤਾਂ {
            IPI_MEM_BARRIER;
            ipi_send_ਨਿਸ਼ਾਨਾ(ipi, cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ID(ਸੂਚਕਾਂਕ));
        }
        ਨਕਾਬ_2 &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ);
    }

    ਜੇ (nr_ਨਿਸ਼ਾਨਾ_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        ਲਈ (ਪੂਰਨ i = 0; i < nr_ਨਿਸ਼ਾਨਾ_cores; i++) {
            ipi_send_ਨਿਸ਼ਾਨਾ(ipi, cpu_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ID(ਨਿਸ਼ਾਨਾ_cores[i]));
        }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_ARM
    ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), cap_ਰਜਿਸਟਰ);
    ਸ਼ਬਦ_t irq = get_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ);
    ਜੇ (ਨਿਸ਼ਾਨਾ > CONFIG_MAX_ਅੰਕ_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    ਜੇ (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_ਨਿਸ਼ਾਨਾ(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, irq), ਦੋਆਧਾਰੀ_ਅੰਕ(ਨਿਸ਼ਾਨਾ));
    ਵਾਪਸ EXCEPTION_NONE;
#ਨਹੀਂਤਾਂ /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#ਜੇ_ਅੰਤ  /* [not] CONFIG_ARCH_ARM */
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
