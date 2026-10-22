/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/invocation.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/notification.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <smp/ipi.h>

exception_t decodeIRQControlInvocation(మాట_t invLabel, మాట_t నిడివి,
                                       cte_t *src_స్థానం, మాట_t *బఫరు)
{
    ఒకవేళ (invLabel == IRQIssueIRQHandler) {
        మాట_t సూచీ, లోతు, irq_w;
        irq_t irq;
        cte_t *dest_స్థానం;
        cap_t cnodeCap;
        lookup_స్థానం_ret_t lu_ret;
        exception_t స్థితి_2;

        ఒకవేళ (నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        irq_w = getSyscallArg(0, బఫరు);
        irq = CORE_IRQ_గమ్యం_IRQT(0, irq_w);
        సూచీ = getSyscallArg(1, బఫరు);
        లోతు = getSyscallArg(2, బఫరు);

        cnodeCap = current_extra_caps.excaprefs[0]->cap;

        స్థితి_2 = Arch_checkIRQ(irq_w);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        ఒకవేళ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("Rejecting request for IRQ %u. Already active.", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి lu_ret.స్థితి_2;
        }
        dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeIRQControl(irq, dest_స్థానం, src_స్థానం);
    } లేకపోతే {
        తిరిగి Arch_decodeIRQControlInvocation(invLabel, నిడివి, src_స్థానం, బఫరు);
    }
}

exception_t invokeIRQControl(irq_t irq, cte_t *handler_స్థానం, cte_t *control_స్థానం)
{
    సమితి_IRQState(IRQSignal, irq);
    cte_చొప్పించు(cap_irq_handler_cap_new(IRQT_గమ్యం_IDX(irq)), control_స్థానం, handler_స్థానం);

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeIRQHandlerInvocation(మాట_t invLabel, irq_t irq)
{
    ఎంపిక (invLabel) {
    స్థితి IRQAckIRQ:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_AckIRQ(irq);
        తిరిగి EXCEPTION_NONE;

    స్థితి IRQSetIRQHandler: {
        cap_t ntfnCap;
        cte_t *స్థానం;

        ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ntfnCap = current_extra_caps.excaprefs[0]->cap;
        స్థానం = current_extra_caps.excaprefs[0];

        ఒకవేళ (cap_get_capType(ntfnCap) != cap_notification_cap ||
            !cap_notification_cap_get_capNtfnCanSend(ntfnCap)) {
            ఒకవేళ (cap_get_capType(ntfnCap) != cap_notification_cap) {
                userError("IRQSetHandler: provided cap is not an notification capability.");
            } లేకపోతే {
                userError("IRQSetHandler: caller does not have send rights on the endpoint.");
            }
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        invoke_IRQHandler_సమితి_IRQHandler(irq, ntfnCap, స్థానం);
        తిరిగి EXCEPTION_NONE;
    }

    స్థితి IRQClearIRQHandler:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        invokeIRQHandler_ClearIRQHandler(irq);
        తిరిగి EXCEPTION_NONE;

    అప్రమేయ:
        userError("IRQHandler: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

శూన్య invokeIRQHandler_AckIRQ(irq_t irq)
{
#నిర్వచితమైతే CONFIG_ARCH_RISCV
#ఒకవేళ !defined(CONFIG_PLAT_QEMU_RISCV_VIRT)
    /* QEMU has a bug where interrupts must be
     * immediately claimed, which is done in getActiveIRQ. For other
     * platforms, the claim can wait and be done here.
     */
    plic_complete_claim(irq);
#అయితే_ముగింపు
#లేకపోతే

#ఒకవేళ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ఒకవేళ (IRQ_IS_PPI(irq) && IRQT_గమ్యం_CORE(irq) != getCurrentCPUIndex()) {
#నిర్వచితమైతే CONFIG_ARM_GIC_V3_SUPPORT
        /* According to the GICv3 spec, SPIs can be deactivated from any PE,
         * but SGIs and PPIs must be deactivated from their target PE.
         */
        do_Remote_Deactivate_వ్యక్తిగత_అంతరాయం(IRQT_గమ్యం_CORE(irq), IRQT_గమ్యం_IDX(irq));
#లేకపోతే /* CONFIG_ARM_GIC_V3_SUPPORT */
        do_Remote_ముసుగు_వ్యక్తిగత_అంతరాయం(IRQT_గమ్యం_CORE(irq), false, IRQT_గమ్యం_IDX(irq));
#అయితే_ముగింపు /* CONFIG_ARM_GIC_V3_SUPPORT */
        తిరిగి;
    }
#అయితే_ముగింపు
    ఒకవేళ (config_సమితి(CONFIG_ARM_GIC_V3_SUPPORT)) {
        deactivate_అంతరాయం(irq);
    } లేకపోతే {
        ముసుగు_అంతరాయం(false, irq);
    }
#అయితే_ముగింపు /* CONFIG_ARCH_RISCV */
}

శూన్య invoke_IRQHandler_సమితి_IRQHandler(irq_t irq, cap_t cap, cte_t *స్థానం)
{
    cte_t *irq_స్థానం;

    irq_స్థానం = intStateIRQNode + IRQT_గమ్యం_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_స్థానం);
    cte_చొప్పించు(cap, స్థానం, irq_స్థానం);
}

శూన్య invokeIRQHandler_ClearIRQHandler(irq_t irq)
{
    cte_t *irq_స్థానం;

    irq_స్థానం = intStateIRQNode + IRQT_గమ్యం_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (-1))" */
    cteDeleteOne(irq_స్థానం);
}

శూన్య deletingIRQHandler(irq_t irq)
{
    cte_t *స్థానం;

    స్థానం = intStateIRQNode + IRQT_గమ్యం_IDX(irq);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_notification_cap))" */
    cteDeleteOne(స్థానం);
}

శూన్య deletedIRQHandler(irq_t irq)
{
    సమితి_IRQState(IRQInactive, irq);
}

శూన్య handle_అంతరాయం(irq_t irq)
{
    ఒకవేళ (unlikely(IRQT_గమ్యం_IRQ(irq) > maxIRQ)) {
        /* The interrupt number is out of range. Pretend it did not happen by
         * handling it like an inactive interrupt (mask and ack). We assume this
         * is acceptable, because the platform specific interrupt controller
         * driver reported this interrupt. Maybe the value maxIRQ is just wrong
         * or set to a lower value because the interrupts are unused.
         */
        రాయి("Received IRQ %d, which is above the platforms maxIRQ of %d\n", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq), (పూర్ణాంకం)maxIRQ);
        ముసుగు_అంతరాయం(true, irq);
        ack_అంతరాయం(irq);
        తిరిగి;
    }

    ఎంపిక (intStateIRQTable[IRQT_గమ్యం_IDX(irq)]) {
    స్థితి IRQSignal: {
        /* Merging the variable declaration and initialization into one line
         * requires an update in the proofs first. Might be a c89 legacy.
         */
        cap_t cap;
        cap = intStateIRQNode[IRQT_గమ్యం_IDX(irq)].cap;
        ఒకవేళ (cap_get_capType(cap) == cap_notification_cap &&
            cap_notification_cap_get_capNtfnCanSend(cap)) {
            sendSignal(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                       cap_notification_cap_get_capNtfnBadge(cap));
        } లేకపోతే {
#నిర్వచితమైతే CONFIG_IRQ_REPORTING
            రాయి("Undelivered IRQ: %d\n", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
#అయితే_ముగింపు
        }
#నిర్వచితం_కాకపోతే CONFIG_ARCH_RISCV
        ఒకవేళ (!config_సమితి(CONFIG_ARM_GIC_V3_SUPPORT)) {
            ముసుగు_అంతరాయం(true, irq);
        }
#అయితే_ముగింపు

        ఆపు;
    }

    స్థితి IRQTimer:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ackDeadlineIRQ();
        కణుపు_STATE(ksReprogram) = true;
#లేకపోతే
        timerTick();
        resetTimer();
#అయితే_ముగింపు
        ఆపు;

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    స్థితి IRQIPI:
        handleIPI(irq, true);
        ఆపు;
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    స్థితి IRQReserved:
        handleReservedIRQ(irq);
        ఆపు;

    స్థితి IRQInactive:
        /* This case shouldn't happen anyway unless the hardware or platform
         * code is broken. Hopefully masking it again should make the interrupt
         * go away.
         */
        ముసుగు_అంతరాయం(true, irq);
#నిర్వచితమైతే CONFIG_IRQ_REPORTING
        రాయి("Received disabled IRQ: %d\n", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
#అయితే_ముగింపు
        ఆపు;

    అప్రమేయ:
        /* No corresponding haskell error */
        fail("Invalid IRQ state");
    }

    /* Every interrupt is ack'd, even if it is an inactive one. Rationale is,
     * that for any interrupt reported by the platform specific code the generic
     * kernel code does report here that it is done with handling it. */
    ack_అంతరాయం(irq);
}

bool_t isIRQActive(irq_t irq)
{
    తిరిగి intStateIRQTable[IRQT_గమ్యం_IDX(irq)] != IRQInactive;
}

శూన్య సమితి_IRQState(irq_state_t irqState, irq_t irq)
{
    intStateIRQTable[IRQT_గమ్యం_IDX(irq)] = irqState;
#ఒకవేళ defined ENABLE_SMP_SUPPORT && defined CONFIG_ARCH_ARM
    ఒకవేళ (IRQ_IS_PPI(irq) && IRQT_గమ్యం_CORE(irq) != getCurrentCPUIndex()) {
        do_Remote_ముసుగు_వ్యక్తిగత_అంతరాయం(IRQT_గమ్యం_CORE(irq), irqState == IRQInactive, IRQT_గమ్యం_IDX(irq));
        తిరిగి;
    }
#అయితే_ముగింపు
    ముసుగు_అంతరాయం(irqState == IRQInactive, irq);
}
