/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/invocation.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <object/interrupt.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <object/notification.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <machine/timer.h>
#അന്തര്ഭാവ <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    എങ്കിൽ (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        എങ്കിൽ (length < 3 || current_extra_caps.excaprefs[0] == ശൂന്യ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
        }

        എങ്കിൽ (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq));
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq));
            തിരികെ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq));
            തിരികെ status;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeIRQControl(irq, destSlot, srcSlot);
    } അല്ലെങ്കിൽ {
        തിരികെ Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_തിരുകുക(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    തിരികെ EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ IRQAckIRQ:
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        തിരികെ EXCEPTION_NONE;

    അവസ്ഥ IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        എങ്കിൽ (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            എങ്കിൽ (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } അല്ലെങ്കിൽ {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        തിരികെ EXCEPTION_NONE;
    }

    അവസ്ഥ IRQClearIRQHandler:
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        തിരികെ EXCEPTION_NONE;

    സ്ഥിരസ്ഥിതി:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

ശൂന്യം invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#എങ്കിൽ !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#അല്ലെങ്കിൽ

#എങ്കിൽ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    എങ്കിൽ (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#അല്ലെങ്കിൽ /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        തിരികെ;
    }
#endif
    എങ്കിൽ (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } അല്ലെങ്കിൽ {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

ശൂന്യം invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_തിരുകുക(cap, slot, irqSlot);
}

ശൂന്യം invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

ശൂന്യം deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

ശൂന്യം deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

ശൂന്യം handleInterrupt(irq_t irq)
{
    എങ്കിൽ (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq), (പൂർണ്ണസംഖ്യ)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        തിരികെ;
    }

    തിരഞ്ഞെടുപ്പ് (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    അവസ്ഥ IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        എങ്കിൽ (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } അല്ലെങ്കിൽ {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        എങ്കിൽ (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        നിർത്തുക;
    }

    അവസ്ഥ IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        കണ്ണി_STATE(ksReprogram) = true;
#അല്ലെങ്കിൽ
        timerTick();
        resetTimer();
#endif
        നിർത്തുക;

#ifdef ENABLE_SMP_SUPPORT
    അവസ്ഥ IRQIPI:
        handleIPI(irq, true);
        നിർത്തുക;
#endif /* ENABLE_SMP_SUPPORT */

    അവസ്ഥ IRQReserved:
        handleReservedIRQ(irq);
        നിർത്തുക;

    അവസ്ഥ IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (പൂർണ്ണസംഖ്യ)IRQT_TO_IRQ(irq));
#endif
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
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
    തിരികെ intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

ശൂന്യം setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#എങ്കിൽ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    എങ്കിൽ (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        തിരികെ;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
