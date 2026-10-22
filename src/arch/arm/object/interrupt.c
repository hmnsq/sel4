/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <config.h>

#अन्तर्भाव <arch/object/interrupt.h>

स्थिर exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
    यदि (config_समुच्चयः(HAVE_समुच्चयः_TRIGGER)) {
        समुच्चयः_IRQTrigger(irq, trigger);
    }
    प्रत्यागम invokeIRQControl(irq, handlerSlot, controlSlot);
}


#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT

स्थिर exception_t Arch_invokeIssueSGISignal(word_t irq, word_t target, cte_t *sgiSlot, cte_t *controlSlot)
{
    cte_निवेशय(cap_sgi_signal_cap_new(target, irq), controlSlot, sgiSlot);
    प्रत्यागम EXCEPTION_NONE;
}

#यदि_अन्तः

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t दीर्घता,
                                            cte_t *srcSlot, word_t *बफर)
{
    यदि (invLabel == ARMIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) CORE_IRQ_गन्तव्यम्_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t गभीरता = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t स्थितिः = Arch_checkIRQ(irq_w);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

#यदि defined ENABLE_SMP_SUPPORT
        यदि (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः
        यदि (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम lu_ret.स्थितिः;
        }

        cte_t *destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    } अन्यथा यदि (invLabel == ARMIRQIssueSGISignal) {
        यदि (दीर्घता < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        यदि (wrap_config_समुच्चयः(अङ्कः_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq = getSyscallArg(0, बफर);
        word_t target = getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t गभीरता = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        यदि (irq >= wrap_config_समुच्चयः(अङ्कः_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = अङ्कः_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (!plat_SGITargetValid(target)) {
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", target);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(बफर, 0));
            प्रत्यागम lu_ret.स्थितिः;
        }
        cte_t *destSlot = lu_ret.slot;

        exception_t स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(बफर, 0));
            प्रत्यागम स्थितिः;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम Arch_invokeIssueSGISignal(irq, target, destSlot, srcSlot);

#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    } अन्यथा यदि (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        word_t irq_w = getSyscallArg(0, बफर);
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t गभीरता = getSyscallArg(3, बफर) & 0xfful;
        seL4_Word target = getSyscallArg(4, बफर);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t स्थितिः = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_गन्तव्यम्_IRQT(target, irq_w);

        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        यदि (target >= CONFIG_MAX_अङ्कः_NODES) {
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            userError("Target core %lu is invalid.", target);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम lu_ret.स्थितिः;
        }

        cte_t *destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णाङ्क)IRQT_गन्तव्यम्_IRQ(irq));
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        यदि (!IRQ_IS_PPI(irq)) {
            समुच्चयः_IRQTarget(irq, target);
        }
        प्रत्यागम Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
    } अन्यथा {
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT

स्थिर exception_t invokeSGISignalGenerate(word_t irq, word_t target)
{
    plat_sendSGI(irq, target);
    प्रत्यागम EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(word_t invLabel, word_t दीर्घता,
                                      cap_t cap, word_t *बफर)
{

    word_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    word_t target = cap_sgi_signal_cap_get_capSGITarget(cap);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSGISignalGenerate(irq, target);
}
#यदि_अन्तः /* !CONFIG_ENABLE_SMP_SUPPORT */
