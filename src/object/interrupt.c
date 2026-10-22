/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/invocation.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/interrupt.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <object/notification.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <machine/timer.h>
#অন্তর্ভাব <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                                       cte_t *srcSlot, word_t *বাফার)
{
    যদি (invLabel == IRQIssueIRQHandler) {
        word_t সূচক, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        যদি (দৈর্ঘ্য < 3 || current_extra_caps.excaprefs[0] == ফাঁকা) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, বাফার);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        সূচক = getSyscallArg(1, বাফার);
        depth = getSyscallArg(2, বাফার);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }

        যদি (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq));
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, সূচক, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(বাফার, 0), (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq));
            ফেরত lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(বাফার, 0), (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq));
            ফেরত status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeIRQControl(irq, destSlot, srcSlot);
    } অন্যথা {
        ফেরত Arch_decodeIRQControlInvocation(invLabel, দৈর্ঘ্য, srcSlot, বাফার);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_ঢোকাও(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    ফেরত EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র IRQAckIRQ:
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        ফেরত EXCEPTION_NONE;

    ক্ষেত্র IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        যদি (current_extra_caps.excaprefs[0] == ফাঁকা) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        যদি (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            যদি (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } অন্যথা {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        ফেরত EXCEPTION_NONE;
    }

    ক্ষেত্র IRQClearIRQHandler:
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        ফেরত EXCEPTION_NONE;

    পূর্বনির্ধারিত:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

শূন্য invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#যদি !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#অন্যথা

#যদি defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    যদি (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#অন্যথা /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        ফেরত;
    }
#endif
    যদি (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } অন্যথা {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

শূন্য invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_ঢোকাও(cap, slot, irqSlot);
}

শূন্য invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

শূন্য deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

শূন্য deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

শূন্য handleInterrupt(irq_t irq)
{
    যদি (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq), (পূর্ণসংখ্যা)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        ফেরত;
    }

    নির্বাচন (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    ক্ষেত্র IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        যদি (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } অন্যথা {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        যদি (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        বিরতি;
    }

    ক্ষেত্র IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        গ্রন্থি_STATE(ksReprogram) = true;
#অন্যথা
        timerTick();
        resetTimer();
#endif
        বিরতি;

#ifdef ENABLE_SMP_SUPPORT
    ক্ষেত্র IRQIPI:
        handleIPI(irq, true);
        বিরতি;
#endif /* ENABLE_SMP_SUPPORT */

    ক্ষেত্র IRQReserved:
        handleReservedIRQ(irq);
        বিরতি;

    ক্ষেত্র IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (পূর্ণসংখ্যা)IRQT_TO_IRQ(irq));
#endif
        বিরতি;

    পূর্বনির্ধারিত:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ackInterrupt(irq);
}

bool_t isIRQActive(irq_t irq)
{
    ফেরত intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

শূন্য setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#যদি defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    যদি (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        ফেরত;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
