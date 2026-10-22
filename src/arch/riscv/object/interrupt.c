/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>

#অন্তর্ভাব <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    যদি (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (পূর্ণসংখ্যা)irq, maxIRQ);
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#ifdef HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#endif
    ফেরত invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                                            cte_t *srcSlot, word_t *বাফার)
{
    যদি (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        যদি (দৈর্ঘ্য < 4 || current_extra_caps.excaprefs[0] == ফাঁকা) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, বাফার);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, বাফার);
        word_t সূচক = getSyscallArg(2, বাফার);
        word_t depth = getSyscallArg(3, বাফার);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }

        যদি (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (পূর্ণসংখ্যা)irq);
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, সূচক, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(বাফার, 0), (পূর্ণসংখ্যা)irq);
            ফেরত lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(বাফার, 0), (পূর্ণসংখ্যা)irq);
            ফেরত status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } অন্যথা {
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}
