/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

#شامل <mode/smp/ipi.h>
#شامل <smp/ipi.h>
#شامل <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
خالی ipiStallCoreCallback(bool_t irq_راستہ)
{
    لفظ_t cpu = getCurrentCPUIndex();
    clh_گرہ_t *گرہ = &big_kernel_تالا.گرہ[cpu];

    اگر (clh_is_self_اندر_قطار() && !irq_راستہ) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        اگر (thread_state_ptr_get_tsType(&گرہ_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_قطار_میں_ڈالو_CURRENT_TCB;
        switch_مقصد_Idle_Thread();
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        commit_وقت();
        گرہ_STATE(ksCurSC) = گرہ_STATE(ksIdleThread)->tcbSchedContext;
#اگر_ختم
        گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        گرہ->ipi = 0;
#اگر_تعریف_شدہ CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#اگر_ختم
        ipi_wait();

        /* Continue waiting on lock */
        جب (گرہ->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            اگر (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                أكد(big_kernel_تالا.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_مقصد_IRQT(cpu, irq_remote_call_ipi), irq_راستہ);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm متلون("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } ورنہ {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_قطار_میں_ڈالو_CURRENT_TCB;
        switch_مقصد_Idle_Thread();
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        commit_وقت();
        گرہ_STATE(ksCurSC) = گرہ_STATE(ksIdleThread)->tcbSchedContext;
#اگر_ختم
        گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

خالی ipi_wait(خالی)
{
    ipi_state_t *ipi = &big_kernel_تالا.ipi;
    لفظ_t cores = ipi->کل_Core_Barrier;
    لفظ_t localsense = ipi->globalsense;
    لفظ_t *گنتی = &ipi->گنتی;

    اگر (__atomic_fetch_add(گنتی, 1, __ATOMIC_ACQ_REL) == cores) {
        *گنتی = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    جب (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

ساکن اندرونی خالی init_ipi_دلائل(IpiRemoteCall_t func,
                                 لفظ_t data1, لفظ_t data2, لفظ_t data3,
                                 لفظ_t نقاب_2)
{
    ipi_state_t *ipi = &big_kernel_تالا.ipi;

    ipi->remoteCall = func;
    ipi->دلائل[0] = data1;
    ipi->دلائل[1] = data2;
    ipi->دلائل[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->کل_Core_Barrier = popcountl(نقاب_2);
}

خالی handleIPI(irq_t irq, bool_t irq_راستہ)
{
    ipi_state_t *ipi = &big_kernel_تالا.ipi;

    اگر (IRQT_مقصد_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->دلائل[0], ipi->دلائل[1], ipi->دلائل[2], irq_راستہ);
    } ورنہ اگر (IRQT_مقصد_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#اگر_تعریف_شدہ CONFIG_ARCH_RISCV
        ifence_local();
#اگر_ختم
    } ورنہ {
        fail("Invalid IPI");
    }
}

خالی do_Remote_نقاب_Op(IpiRemoteCall_t func, لفظ_t data1, لفظ_t data2, لفظ_t data3, لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ثنائی_ہندسہ(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    اگر (نقاب_2 != 0) {
        init_ipi_دلائل(func, data1, data2, data3, نقاب_2);

        /* make sure no resource access passes from this point */
        asm متلون("" ::: "memory");
        ipi_send_نقاب(CORE_IRQ_مقصد_IRQT(0, irq_remote_call_ipi), نقاب_2, true);
        ipi_wait();
    }
}

خالی do_نقاب_Reschedule(لفظ_t نقاب_2)
{
    /* make sure the current core is not set in the mask */
    نقاب_2 &= ~ثنائی_ہندسہ(getCurrentCPUIndex());
    اگر (نقاب_2 != 0) {
        ipi_send_نقاب(CORE_IRQ_مقصد_IRQT(0, irq_reschedule_ipi), نقاب_2, false);
    }
}

خالی generic_ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_ہدف_cores = 0;
    uint16_t ہدف_cores[CONFIG_MAX_عدد_NODES];

    جب (نقاب_2) {
        صحیح اشاریہ = لفظ_بٹ - 1 - clzl(نقاب_2);
        اگر (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_تالا.گرہ[اشاریہ].ipi, 1, __ATOMIC_RELEASE);
            ہدف_cores[nr_ہدف_cores] = اشاریہ;
            nr_ہدف_cores++;
        } ورنہ {
            IPI_MEM_BARRIER;
            ipi_send_ہدف(ipi, cpu_اشاریہ_مقصد_ID(اشاریہ));
        }
        نقاب_2 &= ~ثنائی_ہندسہ(اشاریہ);
    }

    اگر (nr_ہدف_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        برائے (صحیح i = 0; i < nr_ہدف_cores; i++) {
            ipi_send_ہدف(ipi, cpu_اشاریہ_مقصد_ID(ہدف_cores[i]));
        }
    }
}

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(خالی)
{
#اگر_تعریف_شدہ CONFIG_ARCH_ARM
    لفظ_t ہدف = get_رجسٹر(گرہ_STATE(ksCurThread), cap_رجسٹر);
    لفظ_t irq = get_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر);
    اگر (ہدف > CONFIG_MAX_عدد_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    اگر (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_ہدف(CORE_IRQ_مقصد_IRQT(0, irq), ثنائی_ہندسہ(ہدف));
    واپس EXCEPTION_NONE;
#ورنہ /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#اگر_ختم  /* [not] CONFIG_ARCH_ARM */
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */

#اگر_ختم /* ENABLE_SMP_SUPPORT */
