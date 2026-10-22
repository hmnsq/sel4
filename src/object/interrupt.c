/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/invocation.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/interrupt.h>
#அந்தர்பாவ <object/cnode.h>
#அந்தர்பாவ <object/notification.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <machine/timer.h>
#அந்தர்பாவ <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    எனில் (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        எனில் (length < 3 || current_extra_caps.excaprefs[0] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        எனில் (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (முழு)IRQT_TO_IRQ(irq));
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (முழு)IRQT_TO_IRQ(irq));
            திரும்பு lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        எனில் (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (முழு)IRQT_TO_IRQ(irq));
            திரும்பு status;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeIRQControl(irq, destSlot, srcSlot);
    } இல்லையெனில் {
        திரும்பு Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_செருகு(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    திரும்பு EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    தேர்வு (invLabel) {
    நிலை IRQAckIRQ:
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        திரும்பு EXCEPTION_NONE;

    நிலை IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        எனில் (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            எனில் (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } இல்லையெனில் {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        திரும்பு EXCEPTION_NONE;
    }

    நிலை IRQClearIRQHandler:
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        திரும்பு EXCEPTION_NONE;

    இயல்பு:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

வெறுமை invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#எனில் !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#இல்லையெனில்

#எனில் defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    எனில் (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#இல்லையெனில் /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        திரும்பு;
    }
#endif
    எனில் (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } இல்லையெனில் {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

வெறுமை invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_செருகு(cap, slot, irqSlot);
}

வெறுமை invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

வெறுமை deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

வெறுமை deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

வெறுமை handleInterrupt(irq_t irq)
{
    எனில் (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (முழு)IRQT_TO_IRQ(irq), (முழு)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        திரும்பு;
    }

    தேர்வு (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    நிலை IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        எனில் (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } இல்லையெனில் {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (முழு)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        எனில் (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        நிறுத்து;
    }

    நிலை IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        முனை_STATE(ksReprogram) = true;
#இல்லையெனில்
        timerTick();
        resetTimer();
#endif
        நிறுத்து;

#ifdef ENABLE_SMP_SUPPORT
    நிலை IRQIPI:
        handleIPI(irq, true);
        நிறுத்து;
#endif /* ENABLE_SMP_SUPPORT */

    நிலை IRQReserved:
        handleReservedIRQ(irq);
        நிறுத்து;

    நிலை IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (முழு)IRQT_TO_IRQ(irq));
#endif
        நிறுத்து;

    இயல்பு:
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
    திரும்பு intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

வெறுமை setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#எனில் defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    எனில் (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        திரும்பு;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
