/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/invocation.h>
#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <object/structures.h>
#شامل <object/interrupt.h>
#شامل <object/cnode.h>
#شامل <object/notification.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <model/statedata.h>
#شامل <machine/timer.h>
#شامل <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    اگر (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        اگر (length < 3 || current_extra_caps.excaprefs[0] == پوچ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        اگر (status != EXCEPTION_NONE) {
            بازگشت status;
        }

        اگر (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (صحیح)IRQT_TO_IRQ(irq));
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        اگر (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (صحیح)IRQT_TO_IRQ(irq));
            بازگشت lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        اگر (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (صحیح)IRQT_TO_IRQ(irq));
            بازگشت status;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeIRQControl(irq, destSlot, srcSlot);
    } وگرنه {
        بازگشت Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_درج(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    بازگشت EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    انتخاب (invLabel) {
    حالت IRQAckIRQ:
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        بازگشت EXCEPTION_NONE;

    حالت IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        اگر (current_extra_caps.excaprefs[0] == پوچ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        اگر (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            اگر (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } وگرنه {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        بازگشت EXCEPTION_NONE;
    }

    حالت IRQClearIRQHandler:
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        بازگشت EXCEPTION_NONE;

    پیشفرض:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

تهی invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#اگر !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#وگرنه

#اگر defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    اگر (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#وگرنه /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        بازگشت;
    }
#endif
    اگر (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } وگرنه {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

تهی invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_درج(cap, slot, irqSlot);
}

تهی invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

تهی deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

تهی deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

تهی handleInterrupt(irq_t irq)
{
    اگر (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحیح)IRQT_TO_IRQ(irq), (صحیح)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        بازگشت;
    }

    انتخاب (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    حالت IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        اگر (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } وگرنه {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (صحیح)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        اگر (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        بشکن;
    }

    حالت IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        گره_STATE(ksReprogram) = true;
#وگرنه
        timerTick();
        resetTimer();
#endif
        بشکن;

#ifdef ENABLE_SMP_SUPPORT
    حالت IRQIPI:
        handleIPI(irq, true);
        بشکن;
#endif /* ENABLE_SMP_SUPPORT */

    حالت IRQReserved:
        handleReservedIRQ(irq);
        بشکن;

    حالت IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (صحیح)IRQT_TO_IRQ(irq));
#endif
        بشکن;

    پیشفرض:
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
    بازگشت intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

تهی setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#اگر defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    اگر (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        بازگشت;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
