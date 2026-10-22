/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/invocation.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <object/structures.h>
#ضمن <object/interrupt.h>
#ضمن <object/cnode.h>
#ضمن <object/notification.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <model/statedata.h>
#ضمن <machine/timer.h>
#ضمن <smp/ipi.h>

exception_t decodeIRQControlInvocation(word_t invLabel, word_t length,
                                       cte_t *srcSlot, word_t *buffer)
{
    إذا (invLabel == IRQIssueIRQHandler) {
        word_t index, depth, irq_w;
        irq_t irq;
        cte_t *destSlot;
        cap_t cnodeCap;
        lookupSlot_ret_t lu_ret;
        exception_t status;

        إذا (length < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, buffer);
        irq = CORE_IRQ_TO_IRQT(0, irq_w);
        index = getSyscallArg(1, buffer);
        depth = getSyscallArg(2, buffer);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        status = Arch_checkIRQ(irq_w);
        إذا (status != EXCEPTION_NONE) {
            أرجع status;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_TO_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        إذا (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (صحيح)IRQT_TO_IRQ(irq));
            أرجع lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        إذا (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (صحيح)IRQT_TO_IRQ(irq));
            أرجع status;
        }

        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIRQControl(irq, destSlot, srcSlot);
    } وإلا {
        أرجع Arch_decodeIRQControlInvocation(invLabel, length, srcSlot, buffer);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot)
{
    setIRQState(IRQSignal, irq);
    cte_قىستۇرۇش(cap_irq_handler_cap_new(IRQT_TO_IDX(irq)), controlSlot, handlerSlot);

    أرجع EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(word_t invLabel, irq_t irq)
{
    بدل (invLabel) {
    حالة IRQAckIRQ:
        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        أرجع EXCEPTION_NONE;

    حالة IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *slot;

        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        slot = current_extra_caps.excaprefs[0];

        إذا (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            إذا (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } وإلا {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_SetIRQHandler(irq, ntfnCap, slot);
        أرجع EXCEPTION_NONE;
    }

    حالة IRQClearIRQHandler:
        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        أرجع EXCEPTION_NONE;

    افتراضي:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ invokeIRQHandler_AckIRQ(irq_t irq)
{
#ifdef CONFIG_ARCH_RISCV
#إذا !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#endif
#وإلا

#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        doRemoteDeactivatePrivateInterrupt(IRQT_TO_CORE(irq), IRQT_TO_IDX(irq));
#وإلا /* CONFIG_ARM_GIC_V3_SUPPORT */
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), false, IRQT_TO_IDX(irq));
#endif /* CONFIG_ARM_GIC_V3_SUPPORT */
        أرجع;
    }
#endif
    إذا (config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivateInterrupt(irq);
    } وإلا {
        maskInterrupt(false, irq);
    }
#endif /* CONFIG_ARCH_RISCV */
}

فراغ invokeIRQHandler_SetIRQHandler(irq_t irq, cap_t cap, cte_t *slot)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
    cte_قىستۇرۇش(cap, slot, irqSlot);
}

فراغ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irqSlot;

    irqSlot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irqSlot);
}

فراغ deletingIRQHandler(irq_t irq)
{
    cte_t *slot;

    slot = intStateIRQNode + IRQT_TO_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(slot);
}

فراغ deletedIRQHandler(irq_t irq)
{
    setIRQState(IRQInactive, irq);
}

فراغ handleInterrupt(irq_t irq)
{
    إذا (unlikely(IRQT_TO_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        printf("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحيح)IRQT_TO_IRQ(irq), (صحيح)maxIRQ);
        maskInterrupt(true, irq);
        ackInterrupt(irq);
        أرجع;
    }

    بدل (intStateIRQTable[IRQT_TO_IDX(irq)]) {
    حالة IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        إذا (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } وإلا {
#ifdef CONFIG_IRQ_REPORTING
            printf("Undelivered IRQ: %d\n", (صحيح)IRQT_TO_IRQ(irq));
#endif
        }
#ifndef CONFIG_ARCH_RISCV
        إذا (!config_set(CONFIG_ARM_GIC_V3_SUPPORT)) {
            maskInterrupt(true, irq);
        }
#endif

        اكسر;
    }

    حالة IRQTimer:
#ifdef CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        تۈگۈن_STATE(ksReprogram) = true;
#وإلا
        timerTick();
        resetTimer();
#endif
        اكسر;

#ifdef ENABLE_SMP_SUPPORT
    حالة IRQIPI:
        handleIPI(irq, true);
        اكسر;
#endif /* ENABLE_SMP_SUPPORT */

    حالة IRQReserved:
        handleReservedIRQ(irq);
        اكسر;

    حالة IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        maskInterrupt(true, irq);
#ifdef CONFIG_IRQ_REPORTING
        printf("Received disabled IRQ: %d\n", (صحيح)IRQT_TO_IRQ(irq));
#endif
        اكسر;

    افتراضي:
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
    أرجع intStateIRQTable[IRQT_TO_IDX(irq)] != IRQInactive;
}

فراغ setIRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_TO_IDX(irq)] = irqState;
#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_TO_CORE(irq) != getCurrentCPUIndex()) {
        doRemoteMaskPrivateInterrupt(IRQT_TO_CORE(irq), irqState == IRQInactive, IRQT_TO_IDX(irq));
        أرجع;
    }
#endif
    maskInterrupt(irqState == IRQInactive, irq);
}
