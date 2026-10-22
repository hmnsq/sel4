/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <config.h>

#అన్తర్భావ <arch/object/interrupt.h>

స్థిర exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_స్థానం, cte_t *control_స్థానం, bool_t trigger)
{
    ఒకవేళ (config_సమితి(HAVE_సమితి_TRIGGER)) {
        సమితి_IRQTrigger(irq, trigger);
    }
    తిరిగి invokeIRQControl(irq, handler_స్థానం, control_స్థానం);
}


#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT

స్థిర exception_t Arch_invokeIssueSGISignal(మాట_t irq, మాట_t లక్ష్యం, cte_t *sgi_స్థానం, cte_t *control_స్థానం)
{
    cte_చొప్పించు(cap_sgi_signal_cap_new(లక్ష్యం, irq), control_స్థానం, sgi_స్థానం);
    తిరిగి EXCEPTION_NONE;
}

#అయితే_ముగింపు

exception_t Arch_decodeIRQControlInvocation(మాట_t invLabel, మాట_t నిడివి,
                                            cte_t *src_స్థానం, మాట_t *బఫరు)
{
    ఒకవేళ (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        ఒకవేళ (నిడివి < 4 || current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (!config_సమితి(HAVE_సమితి_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        మాట_t irq_w = getSyscallArg(0, బఫరు);
        irq_t irq = (irq_t) CORE_IRQ_గమ్యం_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, బఫరు);
        మాట_t సూచీ = getSyscallArg(2, బఫరు);
        మాట_t లోతు = getSyscallArg(3, బఫరు);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t స్థితి_2 = Arch_checkIRQ(irq_w);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

#ఒకవేళ defined ENABLE_SMP_SUPPORT
        ఒకవేళ (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు
        ఒకవేళ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("Rejecting request for IRQ %u. Already active.", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lookup_స్థానం_ret_t lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి lu_ret.స్థితి_2;
        }

        cte_t *dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి Arch_invokeIRQControl(irq, dest_స్థానం, src_స్థానం, trigger);
#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    } లేకపోతే ఒకవేళ (invLabel == ARMIRQIssueSGISignal) {
        ఒకవేళ (నిడివి < 4 || current_extra_caps.excaprefs[0] == ఖాళీ) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ఒకవేళ (wrap_config_సమితి(అంకె_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        మాట_t irq = getSyscallArg(0, బఫరు);
        మాట_t లక్ష్యం = getSyscallArg(1, బఫరు);
        మాట_t సూచీ = getSyscallArg(2, బఫరు);
        మాట_t లోతు = getSyscallArg(3, బఫరు);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ఒకవేళ (irq >= wrap_config_సమితి(అంకె_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = అంకె_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (!plat_SGITargetValid(లక్ష్యం)) {
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", లక్ష్యం);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lookup_స్థానం_ret_t lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(బఫరు, 0));
            తిరిగి lu_ret.స్థితి_2;
        }
        cte_t *dest_స్థానం = lu_ret.స్థానం;

        exception_t స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(బఫరు, 0));
            తిరిగి స్థితి_2;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి Arch_invokeIssueSGISignal(irq, లక్ష్యం, dest_స్థానం, src_స్థానం);

#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    } లేకపోతే ఒకవేళ (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        మాట_t irq_w = getSyscallArg(0, బఫరు);
        bool_t trigger = !!getSyscallArg(1, బఫరు);
        మాట_t సూచీ = getSyscallArg(2, బఫరు);
        మాట_t లోతు = getSyscallArg(3, బఫరు) & 0xfful;
        se_L4_మాట లక్ష్యం = getSyscallArg(4, బఫరు);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t స్థితి_2 = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_గమ్యం_IRQT(లక్ష్యం, irq_w);

        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        ఒకవేళ (లక్ష్యం >= CONFIG_MAX_అంకె_NODES) {
            current_syscall_error.type = se_L4_Invalid_వాదం;
            userError("Target core %lu is invalid.", లక్ష్యం);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("Rejecting request for IRQ %u. Already active.", (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lookup_స్థానం_ret_t lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి lu_ret.స్థితి_2;
        }

        cte_t *dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)IRQT_గమ్యం_IRQ(irq));
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        ఒకవేళ (!IRQ_IS_PPI(irq)) {
            సమితి_IRQTarget(irq, లక్ష్యం);
        }
        తిరిగి Arch_invokeIRQControl(irq, dest_స్థానం, src_స్థానం, trigger);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
    } లేకపోతే {
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT

స్థిర exception_t invokeSGISignalGenerate(మాట_t irq, మాట_t లక్ష్యం)
{
    plat_sendSGI(irq, లక్ష్యం);
    తిరిగి EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(మాట_t invLabel, మాట_t నిడివి,
                                      cap_t cap, మాట_t *బఫరు)
{

    మాట_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    మాట_t లక్ష్యం = cap_sgi_signal_cap_get_capSGITarget(cap);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeSGISignalGenerate(irq, లక్ష్యం);
}
#అయితే_ముగింపు /* !CONFIG_ENABLE_SMP_SUPPORT */
