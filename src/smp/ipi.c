/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef ENABLE_SMP_SUPPORT

#அந்தர்பாவ <mode/smp/ipi.h>
#அந்தர்பாவ <smp/ipi.h>
#அந்தர்பாவ <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
வெறுமை ipiStallCoreCallback(bool_t irqPath)
{
    word_t cpu = getCurrentCPUIndex();
    clh_முனை_t *முனை = &big_kernel_lock.முனை[cpu];

    எனில் (clh_is_self_in_queue() && !irqPath) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        எனில் (thread_state_ptr_get_tsType(&முனை_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        முனை_STATE(ksCurSC) = முனை_STATE(ksIdleThread)->tcbSchedContext;
#endif
        முனை_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        முனை->ipi = 0;
#ifdef CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#endif
        ipi_wait();

        /* Continue waiting on lock */
        போது (முனை->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            எனில் (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                ப்ரதிஜ்ஞா(big_kernel_lock.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_TO_IRQT(cpu, irq_remote_call_ipi), irqPath);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm நிலையற்ற("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } இல்லையெனில் {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        முனை_STATE(ksCurSC) = முனை_STATE(ksIdleThread)->tcbSchedContext;
#endif
        முனை_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

வெறுமை ipi_wait(வெறுமை)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;
    word_t cores = ipi->totalCoreBarrier;
    word_t localsense = ipi->globalsense;
    word_t *மொத்தம் = &ipi->மொத்தம்;

    எனில் (__atomic_fetch_add(மொத்தம், 1, __ATOMIC_ACQ_REL) == cores) {
        *மொத்தம் = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    போது (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

நிலையான உள்வரி வெறுமை init_ipi_args(IpiRemoteCall_t func,
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

வெறுமை handleIPI(irq_t irq, bool_t irqPath)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    எனில் (IRQT_TO_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->args[0], ipi->args[1], ipi->args[2], irqPath);
    } இல்லையெனில் எனில் (IRQT_TO_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#ifdef CONFIG_ARCH_RISCV
        ifence_local();
#endif
    } இல்லையெனில் {
        fail("Invalid IPI");
    }
}

வெறுமை doRemoteMaskOp(IpiRemoteCall_t func, word_t data1, word_t data2, word_t data3, word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    எனில் (mask != 0) {
        init_ipi_args(func, data1, data2, data3, mask);

        /* make sure no resource access passes from this point */
        asm நிலையற்ற("" ::: "memory");
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_remote_call_ipi), mask, true);
        ipi_wait();
    }
}

வெறுமை doMaskReschedule(word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());
    எனில் (mask != 0) {
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_reschedule_ipi), mask, false);
    }
}

வெறுமை generic_ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_cores = 0;
    uint16_t target_cores[CONFIG_MAX_இலக்கம்_NODES];

    போது (mask) {
        முழு index = wordBits - 1 - clzl(mask);
        எனில் (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_lock.முனை[index].ipi, 1, __ATOMIC_RELEASE);
            target_cores[nr_target_cores] = index;
            nr_target_cores++;
        } இல்லையெனில் {
            IPI_MEM_BARRIER;
            ipi_send_target(ipi, cpuIndexToID(index));
        }
        mask &= ~BIT(index);
    }

    எனில் (nr_target_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        க்கு (முழு i = 0; i < nr_target_cores; i++) {
            ipi_send_target(ipi, cpuIndexToID(target_cores[i]));
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(வெறுமை)
{
#ifdef CONFIG_ARCH_ARM
    word_t target = getRegister(முனை_STATE(ksCurThread), capRegister);
    word_t irq = getRegister(முனை_STATE(ksCurThread), செய்தி_Info_Register);
    எனில் (target > CONFIG_MAX_இலக்கம்_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    எனில் (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_target(CORE_IRQ_TO_IRQT(0, irq), BIT(target));
    திரும்பு EXCEPTION_NONE;
#இல்லையெனில் /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#endif  /* [not] CONFIG_ARCH_ARM */
}
#endif /* CONFIG_DEBUG_BUILD */

#endif /* ENABLE_SMP_SUPPORT */
