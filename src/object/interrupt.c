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

exception_t decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                       cte_t *src_تھاں, لفظ_t *بفر)
{
    إذا (invLabel == IRQIssueIRQHandler) {
        لفظ_t اشاریہ, ڈونگھائی, irq_w;
        irq_t irq;
        cte_t *dest_تھاں;
        cap_t cnodeCap;
        lookup_تھاں_ret_t lu_ret;
        exception_t حالت;

        إذا (لمائی < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, بفر);
        irq = CORE_IRQ_منزل_IRQT(0, irq_w);
        اشاریہ = getSyscallArg(1, بفر);
        ڈونگھائی = getSyscallArg(2, بفر);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }
        dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIRQControl(irq, dest_تھاں, src_تھاں);
    } وإلا {
        أرجع Arch_decodeIRQControlInvocation(invLabel, لمائی, src_تھاں, بفر);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_تھاں, cte_t *control_تھاں)
{
    سیٹ_IRQState(IRQSignal, irq);
    cte_داخل_کرو(cap_irq_handler_cap_new(IRQT_منزل_IDX(irq)), control_تھاں, handler_تھاں);

    أرجع EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(لفظ_t invLabel, irq_t irq)
{
    بدل (invLabel) {
    حالة IRQAckIRQ:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        أرجع EXCEPTION_NONE;

    حالة IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *تھاں;

        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        تھاں = current_extra_caps.excaprefs[0];

        إذا (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            إذا (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } وإلا {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_سیٹ_IRQHandler(irq, ntfnCap, تھاں);
        أرجع EXCEPTION_NONE;
    }

    حالة IRQClearIRQHandler:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
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
#جے_تعریف_شدہ CONFIG_ARCH_RISCV
#إذا !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#جے_انت
#وإلا

#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_منزل_CORE(irq) != getCurrentCPUIndex()) {
#جے_تعریف_شدہ CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_نجی_رکاوٹ(IRQT_منزل_CORE(irq), IRQT_منزل_IDX(irq));
#وإلا /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_نقاب_نجی_رکاوٹ(IRQT_منزل_CORE(irq), false, IRQT_منزل_IDX(irq));
#جے_انت /* CONFIG_ARM_GIC_V3_SUPPORT */
        أرجع;
    }
#جے_انت
    إذا (config_سیٹ(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_رکاوٹ(irq);
    } وإلا {
        نقاب_رکاوٹ(false, irq);
    }
#جے_انت /* CONFIG_ARCH_RISCV */
}

فراغ invoke_IRQHandler_سیٹ_IRQHandler(irq_t irq, cap_t cap, cte_t *تھاں)
{
    cte_t *irq_تھاں;

    irq_تھاں = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_تھاں);
    cte_داخل_کرو(cap, تھاں, irq_تھاں);
}

فراغ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_تھاں;

    irq_تھاں = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_تھاں);
}

فراغ deletingIRQHandler(irq_t irq)
{
    cte_t *تھاں;

    تھاں = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(تھاں);
}

فراغ deletedIRQHandler(irq_t irq)
{
    سیٹ_IRQState(IRQInactive, irq);
}

فراغ handle_رکاوٹ(irq_t irq)
{
    إذا (unlikely(IRQT_منزل_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        لکھو("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحيح)IRQT_منزل_IRQ(irq), (صحيح)maxIRQ);
        نقاب_رکاوٹ(true, irq);
        ack_رکاوٹ(irq);
        أرجع;
    }

    بدل (intStateIRQTable[IRQT_منزل_IDX(irq)]) {
    حالة IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
        إذا (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } وإلا {
#جے_تعریف_شدہ CONFIG_IRQ_REPORTING
            لکھو("Undelivered IRQ: %d\n", (صحيح)IRQT_منزل_IRQ(irq));
#جے_انت
        }
#جے_بنا_تعریف CONFIG_ARCH_RISCV
        إذا (!config_سیٹ(CONFIG_ARM_GIC_V3_SUPPORT)) {
            نقاب_رکاوٹ(true, irq);
        }
#جے_انت

        اكسر;
    }

    حالة IRQTimer:
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        گرہ_STATE(ksReprogram) = true;
#وإلا
        timerTick();
        resetTimer();
#جے_انت
        اكسر;

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    حالة IRQIPI:
        handleIPI(irq, true);
        اكسر;
#جے_انت /* ENABLE_SMP_SUPPORT */

    حالة IRQReserved:
        handleReservedIRQ(irq);
        اكسر;

    حالة IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        نقاب_رکاوٹ(true, irq);
#جے_تعریف_شدہ CONFIG_IRQ_REPORTING
        لکھو("Received disabled IRQ: %d\n", (صحيح)IRQT_منزل_IRQ(irq));
#جے_انت
        اكسر;

    افتراضي:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_رکاوٹ(irq);
}

bool_t isIRQActive(irq_t irq)
{
    أرجع intStateIRQTable[IRQT_منزل_IDX(irq)] != IRQInactive;
}

فراغ سیٹ_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_منزل_IDX(irq)] = irqState;
#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_منزل_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_نقاب_نجی_رکاوٹ(IRQT_منزل_CORE(irq), irqState == IRQInactive, IRQT_منزل_IDX(irq));
        أرجع;
    }
#جے_انت
    نقاب_رکاوٹ(irqState == IRQInactive, irq);
}
