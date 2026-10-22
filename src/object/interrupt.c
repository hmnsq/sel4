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

exception_t decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                       cte_t *src_خانہ, لفظ_t *بفر)
{
    اگر (invLabel == IRQIssueIRQHandler) {
        لفظ_t اشاریہ, گہرائی, irq_w;
        irq_t irq;
        cte_t *dest_خانہ;
        cap_t cnodeCap;
        lookup_خانہ_ret_t lu_ret;
        exception_t حالت_2;

        اگر (لمبائی < 3 || current_extra_caps.excaprefs[0] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, بفر);
        irq = CORE_IRQ_مقصد_IRQT(0, irq_w);
        اشاریہ = getSyscallArg(1, بفر);
        گہرائی = getSyscallArg(2, بفر);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        حالت_2 = Arch_checkIRQ(irq_w);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        اگر (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحیح)IRQT_مقصد_IRQ(irq));
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس lu_ret.حالت_2;
        }
        dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeIRQControl(irq, dest_خانہ, src_خانہ);
    } ورنہ {
        واپس Arch_decodeIRQControlInvocation(invLabel, لمبائی, src_خانہ, بفر);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_خانہ, cte_t *control_خانہ)
{
    سیٹ_IRQState(IRQSignal, irq);
    cte_داخل_کرو(cap_irq_handler_cap_new(IRQT_مقصد_IDX(irq)), control_خانہ, handler_خانہ);

    واپس EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(لفظ_t invLabel, irq_t irq)
{
    انتخاب (invLabel) {
    حالت IRQAckIRQ:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        واپس EXCEPTION_NONE;

    حالت IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *خانہ;

        اگر (current_extra_caps.excaprefs[0] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        خانہ = current_extra_caps.excaprefs[0];

        اگر (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            اگر (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } ورنہ {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_سیٹ_IRQHandler(irq, ntfnCap, خانہ);
        واپس EXCEPTION_NONE;
    }

    حالت IRQClearIRQHandler:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        واپس EXCEPTION_NONE;

    طےشدہ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

خالی invokeIRQHandler_AckIRQ(irq_t irq)
{
#اگر_تعریف_شدہ CONFIG_ARCH_RISCV
#اگر !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#اگر_ختم
#ورنہ

#اگر defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    اگر (IRQ_IS_PPI(irq) && IRQT_مقصد_CORE(irq) != getCurrentCPUIndex()) {
#اگر_تعریف_شدہ CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_نجی_مداخلت(IRQT_مقصد_CORE(irq), IRQT_مقصد_IDX(irq));
#ورنہ /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_نقاب_نجی_مداخلت(IRQT_مقصد_CORE(irq), false, IRQT_مقصد_IDX(irq));
#اگر_ختم /* CONFIG_ARM_GIC_V3_SUPPORT */
        واپس;
    }
#اگر_ختم
    اگر (config_سیٹ(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_مداخلت(irq);
    } ورنہ {
        نقاب_مداخلت(false, irq);
    }
#اگر_ختم /* CONFIG_ARCH_RISCV */
}

خالی invoke_IRQHandler_سیٹ_IRQHandler(irq_t irq, cap_t cap, cte_t *خانہ)
{
    cte_t *irq_خانہ;

    irq_خانہ = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_خانہ);
    cte_داخل_کرو(cap, خانہ, irq_خانہ);
}

خالی invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_خانہ;

    irq_خانہ = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_خانہ);
}

خالی deletingIRQHandler(irq_t irq)
{
    cte_t *خانہ;

    خانہ = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(خانہ);
}

خالی deletedIRQHandler(irq_t irq)
{
    سیٹ_IRQState(IRQInactive, irq);
}

خالی handle_مداخلت(irq_t irq)
{
    اگر (unlikely(IRQT_مقصد_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        لکھو("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحیح)IRQT_مقصد_IRQ(irq), (صحیح)maxIRQ);
        نقاب_مداخلت(true, irq);
        ack_مداخلت(irq);
        واپس;
    }

    انتخاب (intStateIRQTable[IRQT_مقصد_IDX(irq)]) {
    حالت IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
        اگر (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } ورنہ {
#اگر_تعریف_شدہ CONFIG_IRQ_REPORTING
            لکھو("Undelivered IRQ: %d\n", (صحیح)IRQT_مقصد_IRQ(irq));
#اگر_ختم
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARCH_RISCV
        اگر (!config_سیٹ(CONFIG_ARM_GIC_V3_SUPPORT)) {
            نقاب_مداخلت(true, irq);
        }
#اگر_ختم

        توڑو;
    }

    حالت IRQTimer:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        گرہ_STATE(ksReprogram) = true;
#ورنہ
        timerTick();
        resetTimer();
#اگر_ختم
        توڑو;

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    حالت IRQIPI:
        handleIPI(irq, true);
        توڑو;
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    حالت IRQReserved:
        handleReservedIRQ(irq);
        توڑو;

    حالت IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        نقاب_مداخلت(true, irq);
#اگر_تعریف_شدہ CONFIG_IRQ_REPORTING
        لکھو("Received disabled IRQ: %d\n", (صحیح)IRQT_مقصد_IRQ(irq));
#اگر_ختم
        توڑو;

    طےشدہ:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_مداخلت(irq);
}

bool_t isIRQActive(irq_t irq)
{
    واپس intStateIRQTable[IRQT_مقصد_IDX(irq)] != IRQInactive;
}

خالی سیٹ_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_مقصد_IDX(irq)] = irqState;
#اگر defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    اگر (IRQ_IS_PPI(irq) && IRQT_مقصد_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_نقاب_نجی_مداخلت(IRQT_مقصد_CORE(irq), irqState == IRQInactive, IRQT_مقصد_IDX(irq));
        واپس;
    }
#اگر_ختم
    نقاب_مداخلت(irqState == IRQInactive, irq);
}
