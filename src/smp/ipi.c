/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT

#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
فراغ ipiStallCoreCallback(bool_t irq_رستہ)
{
    لفظ_t cpu = getCurrentCPUIndex();
    clh_گرہ_t *گرہ = &big_kernel_جندرا.گرہ[cpu];

    إذا (clh_is_self_وچ_قطار() && !irq_رستہ) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        إذا (thread_state_ptr_get_tsType(&گرہ_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_قطار_وچ_پاؤ_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        commit_ویلا();
        گرہ_STATE(ksCurSC) = گرہ_STATE(ksIdleThread)->tcbSchedContext;
#جے_انت
        گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        گرہ->ipi = 0;
#جے_تعریف_شدہ CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#جے_انت
        ipi_wait();

        /* Continue waiting on lock */
        طالما (گرہ->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            إذا (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                أكد(big_kernel_جندرا.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_منزل_IRQT(cpu, irq_remote_call_ipi), irq_رستہ);
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
        SCHED_قطار_وچ_پاؤ_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        commit_ویلا();
        گرہ_STATE(ksCurSC) = گرہ_STATE(ksIdleThread)->tcbSchedContext;
#جے_انت
        گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

فراغ ipi_wait(فراغ)
{
    ipi_state_t *ipi = &big_kernel_جندرا.ipi;
    لفظ_t cores = ipi->کل_Core_Barrier;
    لفظ_t localsense = ipi->globalsense;
    لفظ_t *گنتی = &ipi->گنتی;

    إذا (__atomic_fetch_add(گنتی, 1, __ATOMIC_ACQ_REL) == cores) {
        *گنتی = 0;
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

ساكن مضمن فراغ init_ipi_دلیلاں(IpiRemoteCall_t func,
                                 لفظ_t data1, لفظ_t data2, لفظ_t data3,
                                 لفظ_t نقاب_2)
{
    ipi_state_t *ipi = &big_kernel_جندرا.ipi;

    ipi->remoteCall = func;
    ipi->دلیلاں[0] = data1;
    ipi->دلیلاں[1] = data2;
    ipi->دلیلاں[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->کل_Core_Barrier = popcountl(نقاب_2);
}

فراغ handleIPI(irq_t irq, bool_t irq_رستہ)
{
    ipi_state_t *ipi = &big_kernel_جندرا.ipi;

    إذا (IRQT_منزل_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->دلیلاں[0], ipi->دلیلاں[1], ipi->دلیلاں[2], irq_رستہ);
    } وإلا إذا (IRQT_منزل_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#جے_تعریف_شدہ CONFIG_ARCH_RISCV
        ifence_local();
#جے_انت
    } وإلا {
        fail("Invalid IPI");
    }
}

فراغ do_Remote_نقاب_Op(IpiRemoteCall_t func, لفظ_t data1, لفظ_t data2, لفظ_t data3, لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ثنائی_ہندسہ(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    إذا (نقاب_2 != 0) {
        init_ipi_دلیلاں(func, data1, data2, data3, نقاب_2);

        /* make sure no resource access passes from this point */
        asm متطاير("" ::: "memory");
        ipi_send_نقاب(CORE_IRQ_منزل_IRQT(0, irq_remote_call_ipi), نقاب_2, true);
        ipi_wait();
    }
}

فراغ do_نقاب_Reschedule(لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ثنائی_ہندسہ(getCurrentCPUIndex());
    إذا (نقاب_2 != 0) {
        ipi_send_نقاب(CORE_IRQ_منزل_IRQT(0, irq_reschedule_ipi), نقاب_2, false);
    }
}

فراغ generic_ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_نشانہ_cores = 0;
    uint16_t نشانہ_cores[CONFIG_MAX_عدد_NODES];

    طالما (نقاب_2) {
        صحيح اشاریہ = لفظ_بٹ - 1 - clzl(نقاب_2);
        إذا (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_جندرا.گرہ[اشاریہ].ipi, 1, __ATOMIC_RELEASE);
            نشانہ_cores[nr_نشانہ_cores] = اشاریہ;
            nr_نشانہ_cores++;
        } وإلا {
            IPI_MEM_BARRIER;
            ipi_send_نشانہ(ipi, cpu_اشاریہ_منزل_ID(اشاریہ));
        }
        نقاب_2 &= ~ثنائی_ہندسہ(اشاریہ);
    }

    إذا (nr_نشانہ_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        لكل (صحيح i = 0; i < nr_نشانہ_cores; i++) {
            ipi_send_نشانہ(ipi, cpu_اشاریہ_منزل_ID(نشانہ_cores[i]));
        }
    }
}

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(فراغ)
{
#جے_تعریف_شدہ CONFIG_ARCH_ARM
    لفظ_t نشانہ = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    لفظ_t irq = get_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر);
    إذا (نشانہ > CONFIG_MAX_عدد_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    إذا (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_نشانہ(CORE_IRQ_منزل_IRQT(0, irq), ثنائی_ہندسہ(نشانہ));
    أرجع EXCEPTION_NONE;
#وإلا /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#جے_انت  /* [not] CONFIG_ARCH_ARM */
}
#جے_انت /* CONFIG_DEBUG_BUILD */

#جے_انت /* ENABLE_SMP_SUPPORT */
