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

exception_t decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t درازی,
                                       cte_t *src_جایگاه, کلمه_t *میانگیر)
{
    إذا (invLabel == IRQIssueIRQHandler) {
        کلمه_t شاخص, عمق, irq_w;
        irq_t irq;
        cte_t *dest_جایگاه;
        cap_t cnodeCap;
        lookup_جایگاه_ret_t lu_ret;
        exception_t وضعیت;

        إذا (درازی < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, میانگیر);
        irq = CORE_IRQ_مقصد_IRQT(0, irq_w);
        شاخص = getSyscallArg(1, میانگیر);
        عمق = getSyscallArg(2, میانگیر);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        وضعیت = Arch_checkIRQ(irq_w);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع lu_ret.وضعیت;
        }
        dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIRQControl(irq, dest_جایگاه, src_جایگاه);
    } وإلا {
        أرجع Arch_decodeIRQControlInvocation(invLabel, درازی, src_جایگاه, میانگیر);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_جایگاه, cte_t *control_جایگاه)
{
    مجموعه_IRQState(IRQSignal, irq);
    cte_درج(cap_irq_handler_cap_new(IRQT_مقصد_IDX(irq)), control_جایگاه, handler_جایگاه);

    أرجع EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(کلمه_t invLabel, irq_t irq)
{
    بدل (invLabel) {
    حالة IRQAckIRQ:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        أرجع EXCEPTION_NONE;

    حالة IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *جایگاه;

        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        جایگاه = current_extra_caps.excaprefs[0];

        إذا (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            إذا (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } وإلا {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_مجموعه_IRQHandler(irq, ntfnCap, جایگاه);
        أرجع EXCEPTION_NONE;
    }

    حالة IRQClearIRQHandler:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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
#اگر_تعریف_شده CONFIG_ARCH_RISCV
#إذا !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#پایان_اگر
#وإلا

#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_مقصد_CORE(irq) != getCurrentCPUIndex()) {
#اگر_تعریف_شده CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_خصوصی_وقفه(IRQT_مقصد_CORE(irq), IRQT_مقصد_IDX(irq));
#وإلا /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_ماسک_خصوصی_وقفه(IRQT_مقصد_CORE(irq), false, IRQT_مقصد_IDX(irq));
#پایان_اگر /* CONFIG_ARM_GIC_V3_SUPPORT */
        أرجع;
    }
#پایان_اگر
    إذا (config_مجموعه(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_وقفه(irq);
    } وإلا {
        ماسک_وقفه(false, irq);
    }
#پایان_اگر /* CONFIG_ARCH_RISCV */
}

فراغ invoke_IRQHandler_مجموعه_IRQHandler(irq_t irq, cap_t cap, cte_t *جایگاه)
{
    cte_t *irq_جایگاه;

    irq_جایگاه = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_جایگاه);
    cte_درج(cap, جایگاه, irq_جایگاه);
}

فراغ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_جایگاه;

    irq_جایگاه = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_جایگاه);
}

فراغ deletingIRQHandler(irq_t irq)
{
    cte_t *جایگاه;

    جایگاه = intStateIRQNode + IRQT_مقصد_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(جایگاه);
}

فراغ deletedIRQHandler(irq_t irq)
{
    مجموعه_IRQState(IRQInactive, irq);
}

فراغ handle_وقفه(irq_t irq)
{
    إذا (unlikely(IRQT_مقصد_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        چاپ_کن("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحيح)IRQT_مقصد_IRQ(irq), (صحيح)maxIRQ);
        ماسک_وقفه(true, irq);
        ack_وقفه(irq);
        أرجع;
    }

    بدل (intStateIRQTable[IRQT_مقصد_IDX(irq)]) {
    حالة IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
        إذا (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } وإلا {
#اگر_تعریف_شده CONFIG_IRQ_REPORTING
            چاپ_کن("Undelivered IRQ: %d\n", (صحيح)IRQT_مقصد_IRQ(irq));
#پایان_اگر
        }
#اگر_تعریف_نشده CONFIG_ARCH_RISCV
        إذا (!config_مجموعه(CONFIG_ARM_GIC_V3_SUPPORT)) {
            ماسک_وقفه(true, irq);
        }
#پایان_اگر

        اكسر;
    }

    حالة IRQTimer:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        گره_STATE(ksReprogram) = true;
#وإلا
        timerTick();
        resetTimer();
#پایان_اگر
        اكسر;

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    حالة IRQIPI:
        handleIPI(irq, true);
        اكسر;
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    حالة IRQReserved:
        handleReservedIRQ(irq);
        اكسر;

    حالة IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        ماسک_وقفه(true, irq);
#اگر_تعریف_شده CONFIG_IRQ_REPORTING
        چاپ_کن("Received disabled IRQ: %d\n", (صحيح)IRQT_مقصد_IRQ(irq));
#پایان_اگر
        اكسر;

    افتراضي:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_وقفه(irq);
}

bool_t isIRQActive(irq_t irq)
{
    أرجع intStateIRQTable[IRQT_مقصد_IDX(irq)] != IRQInactive;
}

فراغ مجموعه_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_مقصد_IDX(irq)] = irqState;
#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_مقصد_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_ماسک_خصوصی_وقفه(IRQT_مقصد_CORE(irq), irqState == IRQInactive, IRQT_مقصد_IDX(irq));
        أرجع;
    }
#پایان_اگر
    ماسک_وقفه(irqState == IRQInactive, irq);
}
