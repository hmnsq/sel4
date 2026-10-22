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

exception_t decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                       cte_t *src_جڳهه, لفظ_t *بفر)
{
    إذا (invLabel == IRQIssueIRQHandler) {
        لفظ_t اشاريو, اونهائي, irq_w;
        irq_t irq;
        cte_t *dest_جڳهه;
        cap_t cnodeCap;
        lookup_جڳهه_ret_t lu_ret;
        exception_t حالت;

        إذا (ڊيگهائي < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, بفر);
        irq = CORE_IRQ_منزل_IRQT(0, irq_w);
        اشاريو = getSyscallArg(1, بفر);
        اونهائي = getSyscallArg(2, بفر);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }
        dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIRQControl(irq, dest_جڳهه, src_جڳهه);
    } وإلا {
        أرجع Arch_decodeIRQControlInvocation(invLabel, ڊيگهائي, src_جڳهه, بفر);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_جڳهه, cte_t *control_جڳهه)
{
    سيٽ_IRQState(IRQSignal, irq);
    cte_وجهو(cap_irq_handler_cap_new(IRQT_منزل_IDX(irq)), control_جڳهه, handler_جڳهه);

    أرجع EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(لفظ_t invLabel, irq_t irq)
{
    بدل (invLabel) {
    حالة IRQAckIRQ:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        أرجع EXCEPTION_NONE;

    حالة IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *جڳهه;

        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        جڳهه = current_extra_caps.excaprefs[0];

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

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_سيٽ_IRQHandler(irq, ntfnCap, جڳهه);
        أرجع EXCEPTION_NONE;
    }

    حالة IRQClearIRQHandler:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
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
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_RISCV
#إذا !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#جيڪڏهن_پورو
#وإلا

#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_منزل_CORE(irq) != getCurrentCPUIndex()) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_ذاتي_مداخلت(IRQT_منزل_CORE(irq), IRQT_منزل_IDX(irq));
#وإلا /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_نقاب_ذاتي_مداخلت(IRQT_منزل_CORE(irq), false, IRQT_منزل_IDX(irq));
#جيڪڏهن_پورو /* CONFIG_ARM_GIC_V3_SUPPORT */
        أرجع;
    }
#جيڪڏهن_پورو
    إذا (config_سيٽ(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_مداخلت(irq);
    } وإلا {
        نقاب_مداخلت(false, irq);
    }
#جيڪڏهن_پورو /* CONFIG_ARCH_RISCV */
}

فراغ invoke_IRQHandler_سيٽ_IRQHandler(irq_t irq, cap_t cap, cte_t *جڳهه)
{
    cte_t *irq_جڳهه;

    irq_جڳهه = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_جڳهه);
    cte_وجهو(cap, جڳهه, irq_جڳهه);
}

فراغ invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_جڳهه;

    irq_جڳهه = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_جڳهه);
}

فراغ deletingIRQHandler(irq_t irq)
{
    cte_t *جڳهه;

    جڳهه = intStateIRQNode + IRQT_منزل_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(جڳهه);
}

فراغ deletedIRQHandler(irq_t irq)
{
    سيٽ_IRQState(IRQInactive, irq);
}

فراغ handle_مداخلت(irq_t irq)
{
    إذا (unlikely(IRQT_منزل_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        لکو("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (صحيح)IRQT_منزل_IRQ(irq), (صحيح)maxIRQ);
        نقاب_مداخلت(true, irq);
        ack_مداخلت(irq);
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
#جيڪڏهن_وصف_ٿيل CONFIG_IRQ_REPORTING
            لکو("Undelivered IRQ: %d\n", (صحيح)IRQT_منزل_IRQ(irq));
#جيڪڏهن_پورو
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARCH_RISCV
        إذا (!config_سيٽ(CONFIG_ARM_GIC_V3_SUPPORT)) {
            نقاب_مداخلت(true, irq);
        }
#جيڪڏهن_پورو

        اكسر;
    }

    حالة IRQTimer:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        ڳنڍ_STATE(ksReprogram) = true;
#وإلا
        timerTick();
        resetTimer();
#جيڪڏهن_پورو
        اكسر;

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    حالة IRQIPI:
        handleIPI(irq, true);
        اكسر;
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    حالة IRQReserved:
        handleReservedIRQ(irq);
        اكسر;

    حالة IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        نقاب_مداخلت(true, irq);
#جيڪڏهن_وصف_ٿيل CONFIG_IRQ_REPORTING
        لکو("Received disabled IRQ: %d\n", (صحيح)IRQT_منزل_IRQ(irq));
#جيڪڏهن_پورو
        اكسر;

    افتراضي:
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
    أرجع intStateIRQTable[IRQT_منزل_IDX(irq)] != IRQInactive;
}

فراغ سيٽ_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_منزل_IDX(irq)] = irqState;
#إذا defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    إذا (IRQ_IS_PPI(irq) && IRQT_منزل_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_نقاب_ذاتي_مداخلت(IRQT_منزل_CORE(irq), irqState == IRQInactive, IRQT_منزل_IDX(irq));
        أرجع;
    }
#جيڪڏهن_پورو
    نقاب_مداخلت(irqState == IRQInactive, irq);
}
