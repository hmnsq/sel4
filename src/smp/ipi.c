/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef ENABLE_SMP_SUPPORT

#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
ಶೂನ್ಯ ipiStallCoreCallback(bool_t irqPath)
{
    word_t cpu = getCurrentCPUIndex();
    clh_ಗಂಟು_t *ಗಂಟು = &big_kernel_lock.ಗಂಟು[cpu];

    ಯದಿ (clh_is_self_in_queue() && !irqPath) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        ಯದಿ (thread_state_ptr_get_tsType(&ಗಂಟು_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksIdleThread)->tcbSchedContext;
#endif
        ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        ಗಂಟು->ipi = 0;
#ifdef CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#endif
        ipi_wait();

        /* Continue waiting on lock */
        ಯಾವತ್ (ಗಂಟು->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            ಯದಿ (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                ಪ್ರತಿಜ್ಞಾ(big_kernel_lock.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_TO_IRQT(cpu, irq_remote_call_ipi), irqPath);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm ಚಞ್ಚಲ("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } ಅನ್ಯಥಾ {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksIdleThread)->tcbSchedContext;
#endif
        ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

ಶೂನ್ಯ ipi_wait(ಶೂನ್ಯ)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;
    word_t cores = ipi->totalCoreBarrier;
    word_t localsense = ipi->globalsense;
    word_t *ಎಣಿಕೆ = &ipi->ಎಣಿಕೆ;

    ಯದಿ (__atomic_fetch_add(ಎಣಿಕೆ, 1, __ATOMIC_ACQ_REL) == cores) {
        *ಎಣಿಕೆ = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    ಯಾವತ್ (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ init_ipi_args(IpiRemoteCall_t func,
                                 word_t data1, word_t data2, word_t data3,
                                 word_t mask)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    ipi->remoteCall = func;
    ipi->args[0] = data1;
    ipi->args[1] = data2;
    ipi->args[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->totalCoreBarrier = popcountl(mask);
}

ಶೂನ್ಯ handleIPI(irq_t irq, bool_t irqPath)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    ಯದಿ (IRQT_TO_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->args[0], ipi->args[1], ipi->args[2], irqPath);
    } ಅನ್ಯಥಾ ಯದಿ (IRQT_TO_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#ifdef CONFIG_ARCH_RISCV
        ifence_local();
#endif
    } ಅನ್ಯಥಾ {
        fail("Invalid IPI");
    }
}

ಶೂನ್ಯ doRemoteMaskOp(IpiRemoteCall_t func, word_t data1, word_t data2, word_t data3, word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    ಯದಿ (mask != 0) {
        init_ipi_args(func, data1, data2, data3, mask);

        /* make sure no resource access passes from this point */
        asm ಚಞ್ಚಲ("" ::: "memory");
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_remote_call_ipi), mask, true);
        ipi_wait();
    }
}

ಶೂನ್ಯ doMaskReschedule(word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());
    ಯದಿ (mask != 0) {
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_reschedule_ipi), mask, false);
    }
}

ಶೂನ್ಯ generic_ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_cores = 0;
    uint16_t target_cores[CONFIG_MAX_ಅಂಕೆ_NODES];

    ಯಾವತ್ (mask) {
        ಪೂರ್ಣಾಙ್ಕ index = wordBits - 1 - clzl(mask);
        ಯದಿ (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_lock.ಗಂಟು[index].ipi, 1, __ATOMIC_RELEASE);
            target_cores[nr_target_cores] = index;
            nr_target_cores++;
        } ಅನ್ಯಥಾ {
            IPI_MEM_BARRIER;
            ipi_send_target(ipi, cpuIndexToID(index));
        }
        mask &= ~BIT(index);
    }

    ಯದಿ (nr_target_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nr_target_cores; i++) {
            ipi_send_target(ipi, cpuIndexToID(target_cores[i]));
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(ಶೂನ್ಯ)
{
#ifdef CONFIG_ARCH_ARM
    word_t target = getRegister(ಗಂಟು_STATE(ksCurThread), capRegister);
    word_t irq = getRegister(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_Register);
    ಯದಿ (target > CONFIG_MAX_ಅಂಕೆ_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    ಯದಿ (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_target(CORE_IRQ_TO_IRQT(0, irq), BIT(target));
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
#ಅನ್ಯಥಾ /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#endif  /* [not] CONFIG_ARCH_ARM */
}
#endif /* CONFIG_DEBUG_BUILD */

#endif /* ENABLE_SMP_SUPPORT */
