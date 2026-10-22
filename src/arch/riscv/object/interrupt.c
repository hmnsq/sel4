/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <api/failures.h>

#समावेश <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    अगर (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (पूर्णांक)irq, maxIRQ);
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    वापस EXCEPTION_NONE;
}

अटल exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#अगर_घोषित HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#पूर्ण_अगर
    वापस invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t लंबाई,
                                            cte_t *srcSlot, word_t *बफर)
{
    अगर (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        अगर (लंबाई < 4 || current_extra_caps.excaprefs[0] == शून्य) {
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, बफर);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t depth = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        अगर (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णांक)irq);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)irq);
            वापस lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)irq);
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } अन्यथा {
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}
