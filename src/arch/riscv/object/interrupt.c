/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>

#अन्तर्भाव <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    यदि (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (पूर्णाङ्क)irq, maxIRQ);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#यदि_परिभाषितम् HAVE_समुच्चयः_TRIGGER
    समुच्चयः_IRQTrigger(irq, trigger);
#यदि_अन्तः
    प्रत्यागम invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t दीर्घता,
                                            cte_t *srcSlot, word_t *बफर)
{
    यदि (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        यदि (दीर्घता < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (!config_समुच्चयः(HAVE_समुच्चयः_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, बफर);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t गभीरता = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t स्थितिः = Arch_checkIRQ(irq_w);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        यदि (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णाङ्क)irq);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)irq);
            प्रत्यागम lu_ret.स्थितिः;
        }

        cte_t *destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)irq);
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } अन्यथा {
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}
