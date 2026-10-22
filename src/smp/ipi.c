/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef ENABLE_SMP_SUPPORT

#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
فراغ ipiStallCoreCallback(bool_t irqPath)
{
    word_t cpu = getCurrentCPUIndex();
    clh_عقدة_t *عقدة = &big_kernel_lock.عقدة[cpu];

    إذا (clh_is_self_in_queue() && !irqPath) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        إذا (thread_state_ptr_get_tsType(&عقدة_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        عقدة_STATE(ksCurSC) = عقدة_STATE(ksIdleThread)->tcbSchedContext;
#endif
        عقدة_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        عقدة->ipi = 0;
#ifdef CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#endif
        ipi_wait();

        /* Continue waiting on lock */
        طالما (عقدة->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            إذا (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                أكد(big_kernel_lock.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_TO_IRQT(cpu, irq_remote_call_ipi), irqPath);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm متطاير("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } وإلا {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ENQUEUE_CURRENT_TCB;
        switchToIdleThread();
#ifdef CONFIG_KERNEL_MCS
        commitTime();
        عقدة_STATE(ksCurSC) = عقدة_STATE(ksIdleThread)->tcbSchedContext;
#endif
        عقدة_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

فراغ ipi_wait(فراغ)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;
    word_t cores = ipi->totalCoreBarrier;
    word_t localsense = ipi->globalsense;
    word_t *عدد = &ipi->عدد;

    إذا (__atomic_fetch_add(عدد, 1, __ATOMIC_ACQ_REL) == cores) {
        *عدد = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    طالما (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

ساكن مضمن فراغ init_ipi_وسائط(IpiRemoteCall_t func,
                                 word_t data1, word_t data2, word_t data3,
                                 word_t mask)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    ipi->remoteCall = func;
    ipi->وسائط[0] = data1;
    ipi->وسائط[1] = data2;
    ipi->وسائط[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->totalCoreBarrier = popcountl(mask);
}

فراغ handleIPI(irq_t irq, bool_t irqPath)
{
    ipi_state_t *ipi = &big_kernel_lock.ipi;

    إذا (IRQT_TO_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->وسائط[0], ipi->وسائط[1], ipi->وسائط[2], irqPath);
    } وإلا إذا (IRQT_TO_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#ifdef CONFIG_ARCH_RISCV
        ifence_local();
#endif
    } وإلا {
        fail("Invalid IPI");
    }
}

فراغ doRemoteMaskOp(IpiRemoteCall_t func, word_t data1, word_t data2, word_t data3, word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    إذا (mask != 0) {
        init_ipi_وسائط(func, data1, data2, data3, mask);

        /* make sure no resource access passes from this point */
        asm متطاير("" ::: "memory");
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_remote_call_ipi), mask, true);
        ipi_wait();
    }
}

فراغ doMaskReschedule(word_t mask)
{
    /* make sure the current core is not set in the mask */
    mask &= ~BIT(getCurrentCPUIndex());
    إذا (mask != 0) {
        ipi_send_mask(CORE_IRQ_TO_IRQT(0, irq_reschedule_ipi), mask, false);
    }
}

فراغ generic_ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_cores = 0;
    uint16_t target_cores[CONFIG_MAX_رقم_NODES];

    طالما (mask) {
        صحيح index = wordBits - 1 - clzl(mask);
        إذا (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_lock.عقدة[index].ipi, 1, __ATOMIC_RELEASE);
            target_cores[nr_target_cores] = index;
            nr_target_cores++;
        } وإلا {
            IPI_MEM_BARRIER;
            ipi_send_target(ipi, cpuIndexToID(index));
        }
        mask &= ~BIT(index);
    }

    إذا (nr_target_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        لكل (صحيح i = 0; i < nr_target_cores; i++) {
            ipi_send_target(ipi, cpuIndexToID(target_cores[i]));
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(فراغ)
{
#ifdef CONFIG_ARCH_ARM
    word_t target = getRegister(عقدة_STATE(ksCurThread), capRegister);
    word_t irq = getRegister(عقدة_STATE(ksCurThread), رسالة_Info_Register);
    إذا (target > CONFIG_MAX_رقم_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    إذا (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_target(CORE_IRQ_TO_IRQT(0, irq), BIT(target));
    أرجع EXCEPTION_NONE;
#وإلا /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#endif  /* [not] CONFIG_ARCH_ARM */
}
#endif /* CONFIG_DEBUG_BUILD */

#endif /* ENABLE_SMP_SUPPORT */
