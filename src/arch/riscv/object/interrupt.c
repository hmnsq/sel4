/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>

#అన్తర్భావ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(మాట_t irq)
{
    ఒకవేళ (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (పూర్ణాంకం)irq, maxIRQ);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_స్థానం, cte_t *control_స్థానం, bool_t trigger)
{
#నిర్వచితమైతే HAVE_సమితి_TRIGGER
    సమితి_IRQTrigger(irq, trigger);
#అయితే_ముగింపు
    తిరిగి invokeIRQControl(irq, handler_స్థానం, control_స్థానం);
}

exception_t Arch_decodeIRQControlInvocation(మాట_t invLabel, మాట_t నిడివి,
                                            cte_t *src_స్థానం, మాట_t *బఫరు)
{
    ఒకవేళ (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, బఫరు);
        మాట_t సూచీ = getSyscallArg(2, బఫరు);
        మాట_t లోతు = getSyscallArg(3, బఫరు);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t స్థితి_2 = Arch_checkIRQ(irq_w);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        ఒకవేళ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("Rejecting request for IRQ %u. Already active.", (పూర్ణాంకం)irq);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        lookup_స్థానం_ret_t lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)irq);
            తిరిగి lu_ret.స్థితి_2;
        }

        cte_t *dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)irq);
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి Arch_invokeIRQControl(irq, dest_స్థానం, src_స్థానం, trigger);
    } లేకపోతే {
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}
