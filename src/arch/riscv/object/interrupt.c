/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>

#અન્તર્ભાવ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    જો (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (પૂર્ણાંક)irq, maxIRQ);
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#ifdef HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#endif
    પાછા invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    જો (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        જો (length < 4 || current_extra_caps.excaprefs[0] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        જો (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (પૂર્ણાંક)irq);
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (પૂર્ણાંક)irq);
            પાછા lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        જો (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (પૂર્ણાંક)irq);
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } નહીંતર {
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}
