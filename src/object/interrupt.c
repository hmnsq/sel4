/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/invocation.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/interrupt.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ଅନ୍ତର୍ଭାଵ <object/notification.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <machine/timer.h>
#ଅନ୍ତର୍ଭାଵ <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    ଯଦି (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        ଯଦି (length < 3 || current_extra_caps.excaprefs[0] == ଖାଲି) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

        ଯଦି (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ status;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeIRQControl(irq, destSlot, srcSlot);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_ଭର୍ତ୍ତି(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    ବାଛ (invLabel) {
    ସ୍ଥିତି IRQAckIRQ:
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        ଫେରନ୍ତୁ EXCEPTION_NONE;

    ସ୍ଥିତି IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        ଯଦି (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            ଯଦି (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } ଅନ୍ୟଥା {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    ସ୍ଥିତି IRQClearIRQHandler:
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        ଫେରନ୍ତୁ EXCEPTION_NONE;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

ଶୂନ୍ୟ invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#ଯଦି !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#ଅନ୍ୟଥା

#ଯଦି defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ଯଦି (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#ଅନ୍ୟଥା /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        ଫେରନ୍ତୁ;
    }
#endif
    ଯଦି (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } ଅନ୍ୟଥା {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

ଶୂନ୍ୟ invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_ଭର୍ତ୍ତି(cap, slot, irqSlot);
}

ଶୂନ୍ୟ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

ଶୂନ୍ୟ deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

ଶୂନ୍ୟ deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

ଶୂନ୍ୟ handleInterrupt(irq_t irq)
{
    ଯଦି (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        ଫେରନ୍ତୁ;
    }

    ବାଛ (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    ସ୍ଥିତି IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        ଯଦି (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } ଅନ୍ୟଥା {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        ଯଦି (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ଗଣ୍ଠି_STATE(ksReprogram) = true;
#ଅନ୍ୟଥା
        timerTick();
        resetTimer();
#endif
        ଭାଙ୍ଗ;

#ifdef ENABLE_SMP_SUPPORT
    ସ୍ଥିତି IRQIPI:
        handleIPI(irq, true);
        ଭାଙ୍ଗ;
#endif /* ENABLE_SMP_SUPPORT */

    ସ୍ଥିତି IRQReserved:
        handleReservedIRQ(irq);
        ଭାଙ୍ଗ;

    ସ୍ଥିତି IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
#endif
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
    ଫେରନ୍ତୁ intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

ଶୂନ୍ୟ setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#ଯଦି defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ଯଦି (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        ଫେରନ୍ତୁ;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
