/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
ಶೂನ್ಯ ipiStallCoreCallback(bool_t irq_ಮಾರ್ಗ)
{
    ಶಬ್ದ_t cpu = getCurrentCPUIndex();
    clh_ಗಂಟು_t *ಗಂಟು = &big_kernel_ಬೀಗ.ಗಂಟು[cpu];

    ಆದರೆ (clh_is_self_ಒಳ_ಸರತಿ() && !irq_ಮಾರ್ಗ) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        ಆದರೆ (thread_state_ptr_get_tsType(&ಗಂಟು_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು_CURRENT_TCB;
        switch_ಗಮ್ಯ_Idle_Thread();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        commit_ಸಮಯ();
        ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksIdleThread)->tcbSchedContext;
#ಆದರೆ_ಅಂತ್ಯ
        ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        ಗಂಟು->ipi = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#ಆದರೆ_ಅಂತ್ಯ
        ipi_wait();

        /* Continue waiting on lock */
        ಆಗ (ಗಂಟು->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            ಆದರೆ (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                ಪ್ರತಿಜ್ಞಾ(big_kernel_ಬೀಗ.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_ಗಮ್ಯ_IRQT(cpu, irq_remote_call_ipi), irq_ಮಾರ್ಗ);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm ಅಸ್ಥಿರ("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು_CURRENT_TCB;
        switch_ಗಮ್ಯ_Idle_Thread();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        commit_ಸಮಯ();
        ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksIdleThread)->tcbSchedContext;
#ಆದರೆ_ಅಂತ್ಯ
        ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

ಶೂನ್ಯ ipi_wait(ಶೂನ್ಯ)
{
    ipi_state_t *ipi = &big_kernel_ಬೀಗ.ipi;
    ಶಬ್ದ_t cores = ipi->ಒಟ್ಟು_Core_Barrier;
    ಶಬ್ದ_t localsense = ipi->globalsense;
    ಶಬ್ದ_t *ಎಣಿಕೆ_2 = &ipi->ಎಣಿಕೆ_2;

    ಆದರೆ (__atomic_fetch_add(ಎಣಿಕೆ_2, 1, __ATOMIC_ACQ_REL) == cores) {
        *ಎಣಿಕೆ_2 = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    ಆಗ (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ init_ipi_ವಾದಗಳು(IpiRemoteCall_t func,
                                 ಶಬ್ದ_t data1, ಶಬ್ದ_t data2, ಶಬ್ದ_t data3,
                                 ಶಬ್ದ_t ಮುಖವಾಡ_2)
{
    ipi_state_t *ipi = &big_kernel_ಬೀಗ.ipi;

    ipi->remoteCall = func;
    ipi->ವಾದಗಳು[0] = data1;
    ipi->ವಾದಗಳು[1] = data2;
    ipi->ವಾದಗಳು[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->ಒಟ್ಟು_Core_Barrier = popcountl(ಮುಖವಾಡ_2);
}

ಶೂನ್ಯ handleIPI(irq_t irq, bool_t irq_ಮಾರ್ಗ)
{
    ipi_state_t *ipi = &big_kernel_ಬೀಗ.ipi;

    ಆದರೆ (IRQT_ಗಮ್ಯ_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->ವಾದಗಳು[0], ipi->ವಾದಗಳು[1], ipi->ವಾದಗಳು[2], irq_ಮಾರ್ಗ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IRQT_ಗಮ್ಯ_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_RISCV
        ifence_local();
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        fail("Invalid IPI");
    }
}

ಶೂನ್ಯ do_Remote_ಮುಖವಾಡ_Op(IpiRemoteCall_t func, ಶಬ್ದ_t data1, ಶಬ್ದ_t data2, ಶಬ್ದ_t data3, ಶಬ್ದ_t ಮುಖವಾಡ_2)
{
    /* make sure the current core is not set in the mask */
    ಮುಖವಾಡ_2 &= ~ಬಿಟ್(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    ಆದರೆ (ಮುಖವಾಡ_2 != 0) {
        init_ipi_ವಾದಗಳು(func, data1, data2, data3, ಮುಖವಾಡ_2);

        /* make sure no resource access passes from this point */
        asm ಅಸ್ಥಿರ("" ::: "memory");
        ipi_send_ಮುಖವಾಡ(CORE_IRQ_ಗಮ್ಯ_IRQT(0, irq_remote_call_ipi), ಮುಖವಾಡ_2, true);
        ipi_wait();
    }
}

ಶೂನ್ಯ do_ಮುಖವಾಡ_Reschedule(ಶಬ್ದ_t ಮುಖವಾಡ_2)
{
    /* make sure the current core is not set in the mask */
    ಮುಖವಾಡ_2 &= ~ಬಿಟ್(getCurrentCPUIndex());
    ಆದರೆ (ಮುಖವಾಡ_2 != 0) {
        ipi_send_ಮುಖವಾಡ(CORE_IRQ_ಗಮ್ಯ_IRQT(0, irq_reschedule_ipi), ಮುಖವಾಡ_2, false);
    }
}

ಶೂನ್ಯ generic_ipi_send_ಮುಖವಾಡ(irq_t ipi, ಶಬ್ದ_t ಮುಖವಾಡ_2, bool_t isBlocking)
{
    ಶಬ್ದ_t nr_ಗುರಿ_cores = 0;
    uint16_t ಗುರಿ_cores[CONFIG_MAX_ಅಂಕೆ_NODES];

    ಆಗ (ಮುಖವಾಡ_2) {
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(ಮುಖವಾಡ_2);
        ಆದರೆ (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_ಬೀಗ.ಗಂಟು[ಸೂಚ್ಯಂಕ].ipi, 1, __ATOMIC_RELEASE);
            ಗುರಿ_cores[nr_ಗುರಿ_cores] = ಸೂಚ್ಯಂಕ;
            nr_ಗುರಿ_cores++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            IPI_MEM_BARRIER;
            ipi_send_ಗುರಿ(ipi, cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ID(ಸೂಚ್ಯಂಕ));
        }
        ಮುಖವಾಡ_2 &= ~ಬಿಟ್(ಸೂಚ್ಯಂಕ);
    }

    ಆದರೆ (nr_ಗುರಿ_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nr_ಗುರಿ_cores; i++) {
            ipi_send_ಗುರಿ(ipi, cpu_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ID(ಗುರಿ_cores[i]));
        }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_ARM
    ಶಬ್ದ_t ಗುರಿ = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), cap_ರಿಜಿಸ್ಟರ್);
    ಶಬ್ದ_t irq = get_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್);
    ಆದರೆ (ಗುರಿ > CONFIG_MAX_ಅಂಕೆ_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    ಆದರೆ (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_ಗುರಿ(CORE_IRQ_ಗಮ್ಯ_IRQT(0, irq), ಬಿಟ್(ಗುರಿ));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
#ಇಲ್ಲದಿದ್ದರೆ /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#ಆದರೆ_ಅಂತ್ಯ  /* [not] CONFIG_ARCH_ARM */
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
