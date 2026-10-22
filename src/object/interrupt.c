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

exception_t decodeIRQControlInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                       cte_t *src_ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (invLabel == IRQIssueIRQHandler) {
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ, irq_w;
        irq_t irq;
        cte_t *dest_ಸ್ಥಾನ;
        cap_t cnodeCap;
        lookup_ಸ್ಥಾನ_ret_t lu_ret;
        exception_t ಸ್ಥಿತಿ_2;

        ಆದರೆ (ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, ಬಫರ್);
        irq = CORE_IRQ_ಗಮ್ಯ_IRQT(0, irq_w);
        ಸೂಚ್ಯಂಕ = getSyscallArg(1, ಬಫರ್);
        ಆಳ = getSyscallArg(2, ಬಫರ್);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        ಸ್ಥಿತಿ_2 = Arch_checkIRQ(irq_w);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಆದರೆ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeIRQControl(irq, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ Arch_decodeIRQControlInvocation(invLabel, ಉದ್ದಳತೆ, src_ಸ್ಥಾನ, ಬಫರ್);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ)
{
    ಗಣ_IRQState(IRQSignal, irq);
    cte_ಸೇರಿಸು(cap_irq_handler_cap_new(IRQT_ಗಮ್ಯ_IDX(irq)), control_ಸ್ಥಾನ, handler_ಸ್ಥಾನ);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(ಶಬ್ದ_t invLabel, irq_t irq)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ IRQAckIRQ:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *ಸ್ಥಾನ;

        ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        ಸ್ಥಾನ = current_extra_caps.excaprefs[0];

        ಆದರೆ (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            ಆದರೆ (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } ಇಲ್ಲದಿದ್ದರೆ {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_ಗಣ_IRQHandler(irq, ntfnCap, ಸ್ಥಾನ);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಸ್ಥಿತಿ IRQClearIRQHandler:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ invokeIRQHandler_AckIRQ(irq_t irq)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_RISCV
#ಆದರೆ !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ

#ಆದರೆ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ಆದರೆ (IRQ_IS_PPI(irq) && IRQT_ಗಮ್ಯ_CORE(irq) != getCurrentCPUIndex()) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_ಖಾಸಗಿ_ಅಡಚಣೆ(IRQT_ಗಮ್ಯ_CORE(irq), IRQT_ಗಮ್ಯ_IDX(irq));
#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_ಮುಖವಾಡ_ಖಾಸಗಿ_ಅಡಚಣೆ(IRQT_ಗಮ್ಯ_CORE(irq), false, IRQT_ಗಮ್ಯ_IDX(irq));
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_GIC_V3_SUPPORT */
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_ಅಡಚಣೆ(irq);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮುಖವಾಡ_ಅಡಚಣೆ(false, irq);
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_RISCV */
}

ಶೂನ್ಯ invoke_IRQHandler_ಗಣ_IRQHandler(irq_t irq, cap_t cap, cte_t *ಸ್ಥಾನ)
{
    cte_t *irq_ಸ್ಥಾನ;

    irq_ಸ್ಥಾನ = intStateIRQNode + IRQT_ಗಮ್ಯ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_ಸ್ಥಾನ);
    cte_ಸೇರಿಸು(cap, ಸ್ಥಾನ, irq_ಸ್ಥಾನ);
}

ಶೂನ್ಯ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_ಸ್ಥಾನ;

    irq_ಸ್ಥಾನ = intStateIRQNode + IRQT_ಗಮ್ಯ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_ಸ್ಥಾನ);
}

ಶೂನ್ಯ deletingIRQHandler(irq_t irq)
{
    cte_t *ಸ್ಥಾನ;

    ಸ್ಥಾನ = intStateIRQNode + IRQT_ಗಮ್ಯ_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(ಸ್ಥಾನ);
}

ಶೂನ್ಯ deletedIRQHandler(irq_t irq)
{
    ಗಣ_IRQState(IRQInactive, irq);
}

ಶೂನ್ಯ handle_ಅಡಚಣೆ(irq_t irq)
{
    ಆದರೆ (unlikely(IRQT_ಗಮ್ಯ_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        ಬರೆ("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq), (ಪೂರ್ಣಾಂಕ)maxIRQ);
        ಮುಖವಾಡ_ಅಡಚಣೆ(true, irq);
        ack_ಅಡಚಣೆ(irq);
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (intStateIRQTable[IRQT_ಗಮ್ಯ_IDX(irq)]) {
    ಸ್ಥಿತಿ IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_ಗಮ್ಯ_IDX(irq)].cap;
        ಆದರೆ (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IRQ_REPORTING
            ಬರೆ("Undelivered IRQ: %d\n", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
#ಆದರೆ_ಅಂತ್ಯ
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARCH_RISCV
        ಆದರೆ (!config_ಗಣ(CONFIG_ARM_GIC_V3_SUPPORT)) {
            ಮುಖವಾಡ_ಅಡಚಣೆ(true, irq);
        }
#ಆದರೆ_ಅಂತ್ಯ

        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ IRQTimer:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ಗಂಟು_STATE(ksReprogram) = true;
#ಇಲ್ಲದಿದ್ದರೆ
        timerTick();
        resetTimer();
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ IRQIPI:
        handleIPI(irq, true);
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    ಸ್ಥಿತಿ IRQReserved:
        handleReservedIRQ(irq);
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        ಮುಖವಾಡ_ಅಡಚಣೆ(true, irq);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IRQ_REPORTING
        ಬರೆ("Received disabled IRQ: %d\n", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_ಅಡಚಣೆ(irq);
}

bool_t isIRQActive(irq_t irq)
{
    ಹಿಂತಿರುಗಿ intStateIRQTable[IRQT_ಗಮ್ಯ_IDX(irq)] != IRQInactive;
}

ಶೂನ್ಯ ಗಣ_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_ಗಮ್ಯ_IDX(irq)] = irqState;
#ಆದರೆ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ಆದರೆ (IRQ_IS_PPI(irq) && IRQT_ಗಮ್ಯ_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_ಮುಖವಾಡ_ಖಾಸಗಿ_ಅಡಚಣೆ(IRQT_ಗಮ್ಯ_CORE(irq), irqState == IRQInactive, IRQT_ಗಮ್ಯ_IDX(irq));
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಮುಖವಾಡ_ಅಡಚಣೆ(irqState == IRQInactive, irq);
}
