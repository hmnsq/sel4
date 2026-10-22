/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>

#അന്തര്ഭാവ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    എങ്കിൽ (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (പൂർണ്ണസംഖ്യ)irq, maxIRQ);
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#ifdef HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#endif
    തിരികെ invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    എങ്കിൽ (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        എങ്കിൽ (length < 4 || current_extra_caps.excaprefs[0] == ശൂന്യ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
        }

        എങ്കിൽ (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (പൂർണ്ണസംഖ്യ)irq);
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (പൂർണ്ണസംഖ്യ)irq);
            തിരികെ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (പൂർണ്ണസംഖ്യ)irq);
            തിരികെ status;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } അല്ലെങ്കിൽ {
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}
