/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
فراغ ipiStallCoreCallback(bool_t irq_رستو)
{
    لفظ_t cpu = getCurrentCPUIndex();
    clh_ڳنڍ_t *ڳنڍ = &big_kernel_تالو.ڳنڍ[cpu];

    إذا (clh_is_self_اندريون_قطار() && !irq_رستو) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        إذا (thread_state_ptr_get_tsType(&ڳنڍ_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_قطار_اندر_وجهو_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        commit_وقت();
        ڳنڍ_STATE(ksCurSC) = ڳنڍ_STATE(ksIdleThread)->tcbSchedContext;
#جيڪڏهن_پورو
        ڳنڍ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        ڳنڍ->ipi = 0;
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#جيڪڏهن_پورو
        ipi_wait();

        /* Continue waiting on lock */
        طالما (ڳنڍ->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            إذا (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                أكد(big_kernel_تالو.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_منزل_IRQT(cpu, irq_remote_call_ipi), irq_رستو);
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
        SCHED_قطار_اندر_وجهو_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        commit_وقت();
        ڳنڍ_STATE(ksCurSC) = ڳنڍ_STATE(ksIdleThread)->tcbSchedContext;
#جيڪڏهن_پورو
        ڳنڍ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

فراغ ipi_wait(فراغ)
{
    ipi_state_t *ipi = &big_kernel_تالو.ipi;
    لفظ_t cores = ipi->ڪل_Core_Barrier;
    لفظ_t localsense = ipi->globalsense;
    لفظ_t *ڳڻپ = &ipi->ڳڻپ;

    إذا (__atomic_fetch_add(ڳڻپ, 1, __ATOMIC_ACQ_REL) == cores) {
        *ڳڻپ = 0;
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

ساكن مضمن فراغ init_ipi_دليلون(IpiRemoteCall_t func,
                                 لفظ_t data1, لفظ_t data2, لفظ_t data3,
                                 لفظ_t نقاب_2)
{
    ipi_state_t *ipi = &big_kernel_تالو.ipi;

    ipi->remoteCall = func;
    ipi->دليلون[0] = data1;
    ipi->دليلون[1] = data2;
    ipi->دليلون[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->ڪل_Core_Barrier = popcountl(نقاب_2);
}

فراغ handleIPI(irq_t irq, bool_t irq_رستو)
{
    ipi_state_t *ipi = &big_kernel_تالو.ipi;

    إذا (IRQT_منزل_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->دليلون[0], ipi->دليلون[1], ipi->دليلون[2], irq_رستو);
    } وإلا إذا (IRQT_منزل_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_RISCV
        ifence_local();
#جيڪڏهن_پورو
    } وإلا {
        fail("Invalid IPI");
    }
}

فراغ do_Remote_نقاب_Op(IpiRemoteCall_t func, لفظ_t data1, لفظ_t data2, لفظ_t data3, لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ٻٽو_انگ(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    إذا (نقاب_2 != 0) {
        init_ipi_دليلون(func, data1, data2, data3, نقاب_2);

        /* make sure no resource access passes from this point */
        asm متطاير("" ::: "memory");
        ipi_send_نقاب(CORE_IRQ_منزل_IRQT(0, irq_remote_call_ipi), نقاب_2, true);
        ipi_wait();
    }
}

فراغ do_نقاب_Reschedule(لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ٻٽو_انگ(getCurrentCPUIndex());
    إذا (نقاب_2 != 0) {
        ipi_send_نقاب(CORE_IRQ_منزل_IRQT(0, irq_reschedule_ipi), نقاب_2, false);
    }
}

فراغ generic_ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_هدف_cores = 0;
    uint16_t هدف_cores[CONFIG_MAX_انگ_NODES];

    طالما (نقاب_2) {
        صحيح اشاريو = لفظ_بٽ - 1 - clzl(نقاب_2);
        إذا (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_تالو.ڳنڍ[اشاريو].ipi, 1, __ATOMIC_RELEASE);
            هدف_cores[nr_هدف_cores] = اشاريو;
            nr_هدف_cores++;
        } وإلا {
            IPI_MEM_BARRIER;
            ipi_send_هدف(ipi, cpu_اشاريو_منزل_ID(اشاريو));
        }
        نقاب_2 &= ~ٻٽو_انگ(اشاريو);
    }

    إذا (nr_هدف_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        لكل (صحيح i = 0; i < nr_هدف_cores; i++) {
            ipi_send_هدف(ipi, cpu_اشاريو_منزل_ID(هدف_cores[i]));
        }
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_ARM
    لفظ_t هدف = get_رجسٽر(ڳنڍ_STATE(ksCurThread), cap_رجسٽر);
    لفظ_t irq = get_رجسٽر(ڳنڍ_STATE(ksCurThread), نياپو_Info_رجسٽر);
    إذا (هدف > CONFIG_MAX_انگ_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    إذا (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_هدف(CORE_IRQ_منزل_IRQT(0, irq), ٻٽو_انگ(هدف));
    أرجع EXCEPTION_NONE;
#وإلا /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#جيڪڏهن_پورو  /* [not] CONFIG_ARCH_ARM */
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
