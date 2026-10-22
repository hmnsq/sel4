/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/notification.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    सचे (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        सचे (length < 3 || current_extra_caps.excaprefs[0] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        सचे (status != EXCEPTION_NONE) {
            पटिनिवत्त status;
        }

        सचे (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णाङ्क)IRQT_TO_IRQ(irq));
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        सचे (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (पूर्णाङ्क)IRQT_TO_IRQ(irq));
            पटिनिवत्त lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        सचे (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (पूर्णाङ्क)IRQT_TO_IRQ(irq));
            पटिनिवत्त status;
        }

        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        पटिनिवत्त invokeIRQControl(irq, destSlot, srcSlot);
    } अञ्ञथा {
        पटिनिवत्त Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_निवेशय(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    विकल्प (invLabel) {
    पक्ष IRQAckIRQ:
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        पटिनिवत्त EXCEPTION_NONE;

    पक्ष IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        सचे (current_extra_caps.excaprefs[0] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        सचे (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            सचे (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } अञ्ञथा {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        पटिनिवत्त EXCEPTION_NONE;
    }

    पक्ष IRQClearIRQHandler:
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        पटिनिवत्त EXCEPTION_NONE;

    स्वभाव:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#सचे !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#अञ्ञथा

#सचे defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    सचे (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#अञ्ञथा /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        पटिनिवत्त;
    }
#endif
    सचे (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } अञ्ञथा {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

शून्य invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_निवेशय(cap, slot, irqSlot);
}

शून्य invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

शून्य deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

शून्य deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

शून्य handleInterrupt(irq_t irq)
{
    सचे (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (पूर्णाङ्क)IRQT_TO_IRQ(irq), (पूर्णाङ्क)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        पटिनिवत्त;
    }

    विकल्प (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    पक्ष IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        सचे (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } अञ्ञथा {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (पूर्णाङ्क)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        सचे (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        भङ्ग;
    }

    पक्ष IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ग्रन्थिः_STATE(ksReprogram) = true;
#अञ्ञथा
        timerTick();
        resetTimer();
#endif
        भङ्ग;

#ifdef ENABLE_SMP_SUPPORT
    पक्ष IRQIPI:
        handleIPI(irq, true);
        भङ्ग;
#endif /* ENABLE_SMP_SUPPORT */

    पक्ष IRQReserved:
        handleReservedIRQ(irq);
        भङ्ग;

    पक्ष IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (पूर्णाङ्क)IRQT_TO_IRQ(irq));
#endif
        भङ्ग;

    स्वभाव:
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
    पटिनिवत्त intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

शून्य setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#सचे defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    सचे (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        पटिनिवत्त;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
