/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/invocation.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <object/interrupt.h>
#અન્તર્ભાવ <object/cnode.h>
#અન્તર્ભાવ <object/notification.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <machine/timer.h>
#અન્તર્ભાવ <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    જો (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        જો (length < 3 || current_extra_caps.excaprefs[0] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        જો (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (પૂર્ણાંક)IRQT_TO_IRQ(irq));
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (પૂર્ણાંક)IRQT_TO_IRQ(irq));
            પાછા lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        જો (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (પૂર્ણાંક)IRQT_TO_IRQ(irq));
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeIRQControl(irq, destSlot, srcSlot);
    } નહીંતર {
        પાછા Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_ઉમેરો(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    પાછા EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    પસંદગી (invLabel) {
    સ્થિતિ IRQAckIRQ:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        પાછા EXCEPTION_NONE;

    સ્થિતિ IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        જો (current_extra_caps.excaprefs[0] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        જો (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            જો (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } નહીંતર {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        પાછા EXCEPTION_NONE;
    }

    સ્થિતિ IRQClearIRQHandler:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        પાછા EXCEPTION_NONE;

    મૂળભૂત:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

શૂન્ય invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#જો !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#નહીંતર

#જો defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    જો (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#નહીંતર /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        પાછા;
    }
#endif
    જો (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } નહીંતર {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

શૂન્ય invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_ઉમેરો(cap, slot, irqSlot);
}

શૂન્ય invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

શૂન્ય deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

શૂન્ય deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

શૂન્ય handleInterrupt(irq_t irq)
{
    જો (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (પૂર્ણાંક)IRQT_TO_IRQ(irq), (પૂર્ણાંક)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        પાછા;
    }

    પસંદગી (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    સ્થિતિ IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        જો (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } નહીંતર {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (પૂર્ણાંક)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        જો (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        તોડો;
    }

    સ્થિતિ IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ગાંઠ_STATE(ksReprogram) = true;
#નહીંતર
        timerTick();
        resetTimer();
#endif
        તોડો;

#ifdef ENABLE_SMP_SUPPORT
    સ્થિતિ IRQIPI:
        handleIPI(irq, true);
        તોડો;
#endif /* ENABLE_SMP_SUPPORT */

    સ્થિતિ IRQReserved:
        handleReservedIRQ(irq);
        તોડો;

    સ્થિતિ IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (પૂર્ણાંક)IRQT_TO_IRQ(irq));
#endif
        તોડો;

    મૂળભૂત:
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
    પાછા intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

શૂન્ય setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#જો defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    જો (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        પાછા;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
