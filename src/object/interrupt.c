/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/notification.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/ipi.h>

exception_t decodeIRQControlInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                       cte_t *src_ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (invLabel == IRQIssueIRQHandler) {
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ, irq_w;
        irq_t irq;
        cte_t *dest_ਥਾਂ;
        cap_t cnodeCap;
        lookup_ਥਾਂ_ret_t lu_ret;
        exception_t ਸਥਿਤੀ;

        ਜੇ (ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, ਬਫਰ);
        irq = CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, irq_w);
        ਸੂਚਕਾਂਕ = getSyscallArg(1, ਬਫਰ);
        ਡੂੰਘਾਈ = getSyscallArg(2, ਬਫਰ);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        ਸਥਿਤੀ = Arch_checkIRQ(irq_w);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਜੇ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("Rejecting request for IRQ %u. Already active.", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeIRQControl(irq, dest_ਥਾਂ, src_ਥਾਂ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ Arch_decodeIRQControlInvocation(invLabel, ਲੰਮਾਈ, src_ਥਾਂ, ਬਫਰ);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_ਥਾਂ, cte_t *control_ਥਾਂ)
{
    ਸੈੱਟ_IRQState(IRQSignal, irq);
    cte_ਪਾਓ(cap_irq_handler_cap_new(IRQT_ਮੰਜ਼ਿਲ_IDX(irq)), control_ਥਾਂ, handler_ਥਾਂ);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(ਸ਼ਬਦ_t invLabel, irq_t irq)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ IRQAckIRQ:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *ਥਾਂ;

        ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        ਥਾਂ = current_extra_caps.excaprefs[0];

        ਜੇ (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            ਜੇ (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } ਨਹੀਂਤਾਂ {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_ਸੈੱਟ_IRQHandler(irq, ntfnCap, ਥਾਂ);
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਹਾਲਤ IRQClearIRQHandler:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        ਵਾਪਸ EXCEPTION_NONE;

    ਮੂਲ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਖਾਲੀ invokeIRQHandler_AckIRQ(irq_t irq)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_RISCV
#ਜੇ !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ

#ਜੇ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ਜੇ (IRQ_IS_PPI(irq) && IRQT_ਮੰਜ਼ਿਲ_CORE(irq) != getCurrentCPUIndex()) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_ਨਿੱਜੀ_ਰੁਕਾਵਟ(IRQT_ਮੰਜ਼ਿਲ_CORE(irq), IRQT_ਮੰਜ਼ਿਲ_IDX(irq));
#ਨਹੀਂਤਾਂ /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_ਨਕਾਬ_ਨਿੱਜੀ_ਰੁਕਾਵਟ(IRQT_ਮੰਜ਼ਿਲ_CORE(irq), false, IRQT_ਮੰਜ਼ਿਲ_IDX(irq));
#ਜੇ_ਅੰਤ /* CONFIG_ARM_GIC_V3_SUPPORT */
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_ਰੁਕਾਵਟ(irq);
    } ਨਹੀਂਤਾਂ {
        ਨਕਾਬ_ਰੁਕਾਵਟ(false, irq);
    }
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_RISCV */
}

ਖਾਲੀ invoke_IRQHandler_ਸੈੱਟ_IRQHandler(irq_t irq, cap_t cap, cte_t *ਥਾਂ)
{
    cte_t *irq_ਥਾਂ;

    irq_ਥਾਂ = intStateIRQNode + IRQT_ਮੰਜ਼ਿਲ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_ਥਾਂ);
    cte_ਪਾਓ(cap, ਥਾਂ, irq_ਥਾਂ);
}

ਖਾਲੀ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_ਥਾਂ;

    irq_ਥਾਂ = intStateIRQNode + IRQT_ਮੰਜ਼ਿਲ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_ਥਾਂ);
}

ਖਾਲੀ deletingIRQHandler(irq_t irq)
{
    cte_t *ਥਾਂ;

    ਥਾਂ = intStateIRQNode + IRQT_ਮੰਜ਼ਿਲ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(ਥਾਂ);
}

ਖਾਲੀ deletedIRQHandler(irq_t irq)
{
    ਸੈੱਟ_IRQState(IRQInactive, irq);
}

ਖਾਲੀ handle_ਰੁਕਾਵਟ(irq_t irq)
{
    ਜੇ (unlikely(IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        ਲਿਖੋ("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq), (ਪੂਰਨ)maxIRQ);
        ਨਕਾਬ_ਰੁਕਾਵਟ(true, irq);
        ack_ਰੁਕਾਵਟ(irq);
        ਵਾਪਸ;
    }

    ਚੋਣ (intStateIRQTable[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)]) {
    ਹਾਲਤ IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)].cap;
        ਜੇ (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IRQ_REPORTING
            ਲਿਖੋ("Undelivered IRQ: %d\n", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
#ਜੇ_ਅੰਤ
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_RISCV
        ਜੇ (!config_ਸੈੱਟ(CONFIG_ARM_GIC_V3_SUPPORT)) {
            ਨਕਾਬ_ਰੁਕਾਵਟ(true, irq);
        }
#ਜੇ_ਅੰਤ

        ਰੋਕੋ;
    }

    ਹਾਲਤ IRQTimer:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ਗੰਢ_STATE(ksReprogram) = true;
#ਨਹੀਂਤਾਂ
        timerTick();
        resetTimer();
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਹਾਲਤ IRQIPI:
        handleIPI(irq, true);
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    ਹਾਲਤ IRQReserved:
        handleReservedIRQ(irq);
        ਰੋਕੋ;

    ਹਾਲਤ IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        ਨਕਾਬ_ਰੁਕਾਵਟ(true, irq);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IRQ_REPORTING
        ਲਿਖੋ("Received disabled IRQ: %d\n", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;

    ਮੂਲ:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_ਰੁਕਾਵਟ(irq);
}

bool_t isIRQActive(irq_t irq)
{
    ਵਾਪਸ intStateIRQTable[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)] != IRQInactive;
}

ਖਾਲੀ ਸੈੱਟ_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)] = irqState;
#ਜੇ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ਜੇ (IRQ_IS_PPI(irq) && IRQT_ਮੰਜ਼ਿਲ_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_ਨਕਾਬ_ਨਿੱਜੀ_ਰੁਕਾਵਟ(IRQT_ਮੰਜ਼ਿਲ_CORE(irq), irqState == IRQInactive, IRQT_ਮੰਜ਼ਿਲ_IDX(irq));
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ
    ਨਕਾਬ_ਰੁਕਾਵਟ(irqState == IRQInactive, irq);
}
