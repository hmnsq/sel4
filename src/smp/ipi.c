/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

#समावेश <mode/smp/ipi.h>
#समावेश <smp/ipi.h>
#समावेश <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
व्योम ipiStallCoreCallback(bool_t irqPath)
{
    word_t cpu = getCurrentCPUIndex();
    clh_गाँठ_t *गाँठ = &big_kernel_lock.गाँठ[cpu];

    अगर (clh_is_self_in_queue() && !irqPath) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        अगर (thread_state_ptr_get_tsType(&गाँठ_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#अगर_घोषित CONFIG_KERNEL_MCS
        commitTime();
        गाँठ_STATE(ksCurSC) = गाँठ_STATE(ksIdleThread)->tcbSchedContext;
#पूर्ण_अगर
        गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        गाँठ->ipi = 0;
#अगर_घोषित CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#पूर्ण_अगर
        ipi_wait();

        /* Continue waiting on lock */
        जबतक (गाँठ->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            अगर (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                निश्चित(big_kernel_lock.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_TO_IRQT(cpu, irq_remote_call_ipi), irqPath);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        यंत्र अस्थिर("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } अन्यथा {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#अगर_घोषित CONFIG_KERNEL_MCS
        commitTime();
        गाँठ_STATE(ksCurSC) = गाँठ_STATE(ksIdleThread)->tcbSchedContext;
#पूर्ण_अगर
        गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

व्योम ipi_wait(व्योम)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;
    word_t cores = ipi->योग_Core_Barrier;
    word_t localsense = ipi->globalsense;
    word_t *गिनती = &ipi->गिनती;

    अगर (__atomic_fetch_add(गिनती, 1, __ATOMIC_ACQ_REL) == cores) {
        *गिनती = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    जबतक (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

अटल अंतरभूत व्योम init_ipi_args(IpiRemoteCall_t func,
                                 word_t data1, word_t data2, word_t data3,
                                 word_t mask)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    ipi->remoteCall = func;
    ipi->args[0] = data1;
    ipi->args[1] = data2;
    ipi->args[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->योग_Core_Barrier = popcountl(mask);
}

व्योम handleIPI(irq_t irq, bool_t irqPath)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    अगर (IRQT_TO_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->args[0], ipi->args[1], ipi->args[2], irqPath);
    } अन्यथा अगर (IRQT_TO_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#अगर_घोषित CONFIG_ARCH_RISCV
        ifence_local();
#पूर्ण_अगर
    } अन्यथा {
        fail("Invalid IPI");
    }
}

व्योम doRemoteMaskOp(IpiRemoteCall_t func, word_t data1, word_t data2, word_t data3, word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    अगर (mask != 0) {
        init_ipi_args(func, data1, data2, data3, mask);

        /* make sure no resource access passes from this point */
        यंत्र अस्थिर("" ::: "memory");
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_remote_call_ipi), mask, true);
        ipi_wait();
    }
}

व्योम doMaskReschedule(word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());
    अगर (mask != 0) {
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_reschedule_ipi), mask, false);
    }
}

व्योम generic_ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_cores = 0;
    uint16_t target_cores[CONFIG_MAX_अंक_NODES];

    जबतक (mask) {
        पूर्णांक सूचकांक = wordBits - 1 - clzl(mask);
        अगर (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_lock.गाँठ[सूचकांक].ipi, 1, __ATOMIC_RELEASE);
            target_cores[nr_target_cores] = सूचकांक;
            nr_target_cores++;
        } अन्यथा {
            IPI_MEM_BARRIER;
            ipi_send_target(ipi, cpu_सूचकांक_To_ID(सूचकांक));
        }
        mask &= ~BIT(सूचकांक);
    }

    अगर (nr_target_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        क्रम (पूर्णांक i = 0; i < nr_target_cores; i++) {
            ipi_send_target(ipi, cpu_सूचकांक_To_ID(target_cores[i]));
        }
    }
}

#अगर_घोषित CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(व्योम)
{
#अगर_घोषित CONFIG_ARCH_ARM
    word_t target = getRegister(गाँठ_STATE(ksCurThread), capRegister);
    word_t irq = getRegister(गाँठ_STATE(ksCurThread), संदेश_Info_Register);
    अगर (target > CONFIG_MAX_अंक_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    अगर (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_target(CORE_IRQ_TO_IRQT(0, irq), BIT(target));
    वापस EXCEPTION_NONE;
#अन्यथा /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#पूर्ण_अगर  /* [not] CONFIG_ARCH_ARM */
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
