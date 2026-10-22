/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT

#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <smp/ipi.h>
#अन्तर्भाव <smp/lock.h>

/* This function switches the core it is called on to the idle thread,
 * in order to avoid IPI storms. If the core is waiting on the lock, the actual
 * switch will not occur until the core attempts to obtain the lock, at which
 * point the core will capture the pending IPI, which is discarded.

 * The core who triggered the store is responsible for triggering a reschedule,
 * or this call will idle forever */
शून्य ipiStallCoreCallback(bool_t irq_मार्गः)
{
    word_t cpu = getCurrentCPUIndex();
    clh_ग्रन्थिः_t *ग्रन्थिः = &big_kernel_तालकम्.ग्रन्थिः[cpu];

    यदि (clh_is_self_अन्तर्_queue() && !irq_मार्गः) {
        /* The current thread is running as we would replace this thread with an idle thread
         *
         * The instruction should be re-executed if we are in kernel to handle syscalls.
         * Also, thread in 'ThreadState_RunningVM' should remain in same state.
         * Note that, 'ThreadState_Restart' does not always result in regenerating exception
         * if we are in kernel to handle them, e.g. hardware single step exception. */
        यदि (thread_state_ptr_get_tsType(&ग्रन्थिः_STATE(ksCurThread)->tcbState) == ThreadState_Running) {
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        }

        SCHED_पङ्क्तौ_निक्षिप_CURRENT_TCB;
        switch_गन्तव्यम्_Idle_Thread();
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        commitTime();
        ग्रन्थिः_STATE(ksCurSC) = ग्रन्थिः_STATE(ksIdleThread)->tcbSchedContext;
#यदि_अन्तः
        ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;

        /* Let the cpu requesting this IPI continue while we wait on the lock */
        ग्रन्थिः->ipi = 0;
#यदि_परिभाषितम् CONFIG_ARCH_RISCV
        ipi_clear_irq(irq_remote_call_ipi);
#यदि_अन्तः
        ipi_wait();

        /* Continue waiting on lock */
        यावत् (ग्रन्थिः->watch->state != CLHState_Granted) {
            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            यदि (clh_is_ipi_pending(cpu)) {
                /* Multiple calls for similar reason could result in stack overflow */
                प्रतिज्ञा(big_kernel_तालकम्.ipi.remoteCall != IpiRemoteCall_Stall);
                handleIPI(CORE_IRQ_गन्तव्यम्_IRQT(cpu, irq_remote_call_ipi), irq_मार्गः);
            }
            arch_pause();
        }

        /* make sure no resource access passes from this point */
        asm चञ्चल("" ::: "memory");

        /* Start idle thread to capture the pending IPI */
        activateThread();
        restore_user_context();
    } अन्यथा {
        /* We get here either without grabbing the lock from normal interrupt path or from
         * inside the lock while waiting to grab the lock for handling pending interrupt.
         * In latter case, we return to the 'clh_lock_acquire' to grab the lock and
         * handle the pending interrupt. Its valid as interrupts are async events! */
        SCHED_पङ्क्तौ_निक्षिप_CURRENT_TCB;
        switch_गन्तव्यम्_Idle_Thread();
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        commitTime();
        ग्रन्थिः_STATE(ksCurSC) = ग्रन्थिः_STATE(ksIdleThread)->tcbSchedContext;
#यदि_अन्तः
        ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
    }
}

शून्य ipi_wait(शून्य)
{
    ipi_state_t *ipi = &big_kernel_तालकम्.ipi;
    word_t cores = ipi->योग_Core_Barrier;
    word_t localsense = ipi->globalsense;
    word_t *गणनम् = &ipi->गणनम्;

    यदि (__atomic_fetch_add(गणनम्, 1, __ATOMIC_ACQ_REL) == cores) {
        *गणनम् = 0;
        ipi->globalsense++;
        __atomic_thread_fence(__ATOMIC_RELEASE);
    }
    /* Check globalsense instead of count to protect against a race where
     * a new IPI started before this core saw that the old one finished. */
    यावत् (localsense == ipi->globalsense) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        arch_pause();
    }
}

स्थिर अन्तःस्थ शून्य init_ipi_तर्काः(IpiRemoteCall_t func,
                                 word_t data1, word_t data2, word_t data3,
                                 word_t आवरणम्_2)
{
    ipi_state_t *ipi = &big_kernel_तालकम्.ipi;

    ipi->remoteCall = func;
    ipi->तर्काः[0] = data1;
    ipi->तर्काः[1] = data2;
    ipi->तर्काः[2] = data3;

    /* get number of cores involved in this IPI */
    ipi->योग_Core_Barrier = popcountl(आवरणम्_2);
}

शून्य handleIPI(irq_t irq, bool_t irq_मार्गः)
{
    ipi_state_t *ipi = &big_kernel_तालकम्.ipi;

    यदि (IRQT_गन्तव्यम्_IRQ(irq) == irq_remote_call_ipi) {
        handleRemoteCall(ipi->remoteCall, ipi->तर्काः[0], ipi->तर्काः[1], ipi->तर्काः[2], irq_मार्गः);
    } अन्यथा यदि (IRQT_गन्तव्यम्_IRQ(irq) == irq_reschedule_ipi) {
        rescheduleRequired();
#यदि_परिभाषितम् CONFIG_ARCH_RISCV
        ifence_local();
#यदि_अन्तः
    } अन्यथा {
        fail("Invalid IPI");
    }
}

शून्य do_Remote_आवरणम्_Op(IpiRemoteCall_t func, word_t data1, word_t data2, word_t data3, word_t आवरणम्_2)
{
    /* make sure the current core is not set in the mask */
    आवरणम्_2 &= ~BIT(getCurrentCPUIndex());

    /* this may happen, e.g. the caller tries to map a pagetable in
     * newly created PD which has not been run yet. Guard against them! */
    यदि (आवरणम्_2 != 0) {
        init_ipi_तर्काः(func, data1, data2, data3, आवरणम्_2);

        /* make sure no resource access passes from this point */
        asm चञ्चल("" ::: "memory");
        ipi_send_आवरणम्(CORE_IRQ_गन्तव्यम्_IRQT(0, irq_remote_call_ipi), आवरणम्_2, true);
        ipi_wait();
    }
}

शून्य do_आवरणम्_Reschedule(word_t आवरणम्_2)
{
    /* make sure the current core is not set in the mask */
    आवरणम्_2 &= ~BIT(getCurrentCPUIndex());
    यदि (आवरणम्_2 != 0) {
        ipi_send_आवरणम्(CORE_IRQ_गन्तव्यम्_IRQT(0, irq_reschedule_ipi), आवरणम्_2, false);
    }
}

शून्य generic_ipi_send_आवरणम्(irq_t ipi, word_t आवरणम्_2, bool_t isBlocking)
{
    word_t nr_target_cores = 0;
    uint16_t target_cores[CONFIG_MAX_अङ्कः_NODES];

    यावत् (आवरणम्_2) {
        पूर्णाङ्क सूचकांक = word_द्वयङ्काः - 1 - clzl(आवरणम्_2);
        यदि (isBlocking) {
            /*
             * All writes before setting ipi to 1 must be observed,
             * as other cores may check the ipi flag at any moment.
             * IPI_MEM_BARRIER is too late to prevent reordering
             * between IPI data and flag reads.
             */
            __atomic_store_n(&big_kernel_तालकम्.ग्रन्थिः[सूचकांक].ipi, 1, __ATOMIC_RELEASE);
            target_cores[nr_target_cores] = सूचकांक;
            nr_target_cores++;
        } अन्यथा {
            IPI_MEM_BARRIER;
            ipi_send_target(ipi, cpu_सूचकांक_गन्तव्यम्_ID(सूचकांक));
        }
        आवरणम्_2 &= ~BIT(सूचकांक);
    }

    यदि (nr_target_cores > 0) {
        /* sending IPIs... */
        IPI_MEM_BARRIER;
        प्रति (पूर्णाङ्क i = 0; i < nr_target_cores; i++) {
            ipi_send_target(ipi, cpu_सूचकांक_गन्तव्यम्_ID(target_cores[i]));
        }
    }
}

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
exception_t handle_SysDebugSendIPI(शून्य)
{
#यदि_परिभाषितम् CONFIG_ARCH_ARM
    word_t target = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), cap_पञ्जिकाकोशः);
    word_t irq = get_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_पञ्जिकाकोशः);
    यदि (target > CONFIG_MAX_अङ्कः_NODES) {
        userError("SysDebugSendIPI: Invalid target, halting");
        halt();
    }
    यदि (irq > 15) {
        userError("SysDebugSendIPI: Invalid IRQ, not a SGI, halting");
        halt();
    }
    ipi_send_target(CORE_IRQ_गन्तव्यम्_IRQT(0, irq), BIT(target));
    प्रत्यागम EXCEPTION_NONE;
#अन्यथा /* not CONFIG_ARCH_ARM */
    userError("SysDebugSendIPI: not supported on this architecture");
    halt();
#यदि_अन्तः  /* [not] CONFIG_ARCH_ARM */
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
