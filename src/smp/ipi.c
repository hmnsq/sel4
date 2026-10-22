/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
فراغ ipiStallCoreCallback(bool_t irq_لار)
{
    کلمه_t cpu = getCurrentCPUIndex();
    clh_غوټه_t *غوټه = &big_kernel_قلف.غوټه[cpu];

    إذا (clh_is_self_دننه_کتار() && !irq_لار) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        إذا (thread_state_ptr_get_tsType(&غوټه_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_کتار_ته_ورزیاتول_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#که_تعریف_شوی CONFIG_KERNEL_MCS
        commit_وخت();
        غوټه_STATE(ksCurSC) = غوټه_STATE(ksIdleThread)->tcbSchedContext;
#د_که_پای
        غوټه_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        غوټه->ipi = 0;
#که_تعریف_شوی CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#د_که_پای
        ipi_wait();

        /* Continue waiting on lock */
        طالما (غوټه->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            إذا (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                أكد(big_kernel_قلف.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_منزل_IRQT(cpu, irq_remote_call_ipi), irq_لار);
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
        SCHED_کتار_ته_ورزیاتول_CURRENT_TCB;
        switch_منزل_Idle_Thread();
#که_تعریف_شوی CONFIG_KERNEL_MCS
        commit_وخت();
        غوټه_STATE(ksCurSC) = غوټه_STATE(ksIdleThread)->tcbSchedContext;
#د_که_پای
        غوټه_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

فراغ ipi_wait(فراغ)
{
    ipi_state_t *ipi = &big_kernel_قلف.ipi;
    کلمه_t cores = ipi->ټول_Core_Barrier;
    کلمه_t localsense = ipi->globalsense;
    کلمه_t *شمېر = &ipi->شمېر;

    إذا (__atomic_fetch_add(شمېر, 1, __ATOMIC_ACQ_REL) == cores) {
        *شمېر = 0;
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

ساكن مضمن فراغ init_ipi_دلیلونه(IpiRemoteCall_t func,
                                 کلمه_t data1, کلمه_t data2, کلمه_t data3,
                                 کلمه_t ماسک_2)
{
    ipi_state_t *ipi = &big_kernel_قلف.ipi;

    ipi->remoteCall = func;
    ipi->دلیلونه[0] = data1;
    ipi->دلیلونه[1] = data2;
    ipi->دلیلونه[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->ټول_Core_Barrier = popcountl(ماسک_2);
}

فراغ handleIPI(irq_t irq, bool_t irq_لار)
{
    ipi_state_t *ipi = &big_kernel_قلف.ipi;

    إذا (IRQT_منزل_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->دلیلونه[0], ipi->دلیلونه[1], ipi->دلیلونه[2], irq_لار);
    } وإلا إذا (IRQT_منزل_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#که_تعریف_شوی CONFIG_ARCH_RISCV
        ifence_local();
#د_که_پای
    } وإلا {
        fail("Invalid IPI");
    }
}

فراغ do_Remote_ماسک_Op(IpiRemoteCall_t func, کلمه_t data1, کلمه_t data2, کلمه_t data3, کلمه_t ماسک_2)
{
    /* make sure the current core is not set in the mask */
    ماسک_2 &= ~بټ(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    إذا (ماسک_2 != 0) {
        init_ipi_دلیلونه(func, data1, data2, data3, ماسک_2);

        /* make sure no resource access passes from this point */
        asm متطاير("" ::: "memory");
        ipi_send_ماسک(CORE_IRQ_منزل_IRQT(0, irq_remote_call_ipi), ماسک_2, true);
        ipi_wait();
    }
}

فراغ do_ماسک_Reschedule(کلمه_t ماسک_2)
{
    /* make sure the current core is not set in the mask */
    ماسک_2 &= ~بټ(getCurrentCPUIndex());
    إذا (ماسک_2 != 0) {
        ipi_send_ماسک(CORE_IRQ_منزل_IRQT(0, irq_reschedule_ipi), ماسک_2, false);
    }
}

فراغ generic_ipi_send_ماسک(irq_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    کلمه_t nr_موخه_cores = 0;
    uint16_t موخه_cores[CONFIG_MAX_عدد_NODES];

    طالما (ماسک_2) {
        صحيح شاخص = کلمه_بټونه - 1 - clzl(ماسک_2);
        إذا (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_قلف.غوټه[شاخص].ipi, 1, __ATOMIC_RELEASE);
            موخه_cores[nr_موخه_cores] = شاخص;
            nr_موخه_cores++;
        } وإلا {
            IPI_MEM_BARRIER;
            ipi_send_موخه(ipi, cpu_شاخص_منزل_ID(شاخص));
        }
        ماسک_2 &= ~بټ(شاخص);
    }

    إذا (nr_موخه_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        لكل (صحيح i = 0; i < nr_موخه_cores; i++) {
            ipi_send_موخه(ipi, cpu_شاخص_منزل_ID(موخه_cores[i]));
        }
    }
}

#که_تعریف_شوی CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(فراغ)
{
#که_تعریف_شوی CONFIG_ARCH_ARM
    کلمه_t موخه = get_ثبتوونکی(غوټه_STATE(ksCurThread), cap_ثبتوونکی);
    کلمه_t irq = get_ثبتوونکی(غوټه_STATE(ksCurThread), پیغام_Info_ثبتوونکی);
    إذا (موخه > CONFIG_MAX_عدد_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    إذا (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_موخه(CORE_IRQ_منزل_IRQT(0, irq), بټ(موخه));
    أرجع EXCEPTION_NONE;
#وإلا /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#د_که_پای  /* [not] CONFIG_ARCH_ARM */
}
#د_که_پای /* CONFIG_DEBUG_BUILD */

#د_که_پای /* ENABLE_SMP_SUPPORT */
