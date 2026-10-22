/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/invocation.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/notification.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    ಯದಿ (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        ಯದಿ (length < 3 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        ಯದಿ (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeIRQControl(irq, destSlot, srcSlot);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_ಸೇರಿಸು(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ IRQAckIRQ:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;

    ಪಕ್ಷ IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        ಯದಿ (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            ಯದಿ (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } ಅನ್ಯಥಾ {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    ಪಕ್ಷ IRQClearIRQHandler:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;

    ಸ್ವಭಾವ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#ಯದಿ !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#ಅನ್ಯಥಾ

#ಯದಿ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ಯದಿ (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#ಅನ್ಯಥಾ /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    ಯದಿ (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } ಅನ್ಯಥಾ {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

ಶೂನ್ಯ invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_ಸೇರಿಸು(cap, slot, irqSlot);
}

ಶೂನ್ಯ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

ಶೂನ್ಯ deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

ಶೂನ್ಯ deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

ಶೂನ್ಯ handleInterrupt(irq_t irq)
{
    ಯದಿ (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq), (ಪೂರ್ಣಾಙ್ಕ)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        ಪ್ರತ್ಯಾಗಮ;
    }

    ವಿಕಲ್ಪ (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    ಪಕ್ಷ IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        ಯದಿ (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } ಅನ್ಯಥಾ {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        ಯದಿ (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        ಭಙ್ಗ;
    }

    ಪಕ್ಷ IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ಗಂಟು_STATE(ksReprogram) = true;
#ಅನ್ಯಥಾ
        timerTick();
        resetTimer();
#endif
        ಭಙ್ಗ;

#ifdef ENABLE_SMP_SUPPORT
    ಪಕ್ಷ IRQIPI:
        handleIPI(irq, true);
        ಭಙ್ಗ;
#endif /* ENABLE_SMP_SUPPORT */

    ಪಕ್ಷ IRQReserved:
        handleReservedIRQ(irq);
        ಭಙ್ಗ;

    ಪಕ್ಷ IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
#endif
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
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
    ಪ್ರತ್ಯಾಗಮ intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

ಶೂನ್ಯ setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#ಯದಿ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ಯದಿ (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        ಪ್ರತ್ಯಾಗಮ;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
