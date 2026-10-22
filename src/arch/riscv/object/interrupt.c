/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>

#ଅନ୍ତର୍ଭାଵ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    ଯଦି (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq, maxIRQ);
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#ifdef HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#endif
    ଫେରନ୍ତୁ invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    ଯଦି (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        ଯଦି (length < 4 || current_extra_caps.excaprefs[0] == ଖାଲି) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

        ଯଦି (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq);
            ଫେରନ୍ତୁ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)irq);
            ଫେରନ୍ତୁ status;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } ଅନ୍ୟଥା {
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}
