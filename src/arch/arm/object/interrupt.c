/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <config.h>

#समावेश <arch/object/interrupt.h>

अटल exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
    अगर (config_set(HAVE_SET_TRIGGER)) {
        setIRQTrigger(irq, trigger);
    }
    वापस invokeIRQControl(irq, handlerSlot, controlSlot);
}


#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT

अटल exception_t Arch_invokeIssueSGISignal(word_t irq, word_t target, cte_t *sgiSlot, cte_t *controlSlot)
{
    cte_जोड़ो(cap_sgi_signal_cap_new(target, irq), controlSlot, sgiSlot);
    वापस EXCEPTION_NONE;
}

#पूर्ण_अगर

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t लंबाई,
                                            cte_t *srcSlot, word_t *बफर)
{
    अगर (invLabel == ARMIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t depth = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

#अगर defined ENABLE_SMP_SUPPORT
        अगर (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर
        अगर (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    } अन्यथा अगर (invLabel == ARMIRQIssueSGISignal) {
        अगर (लंबाई < 4 || current_extra_caps.excaprefs[0] == शून्य) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        अगर (wrap_config_set(अंक_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq = getSyscallArg(0, बफर);
        word_t target = getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t depth = getSyscallArg(3, बफर);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        अगर (irq >= wrap_config_set(अंक_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = अंक_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (!plat_SGITargetValid(target)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", target);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(बफर, 0));
            वापस lu_ret.status;
        }
        cte_t *destSlot = lu_ret.slot;

        exception_t status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(बफर, 0));
            वापस status;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस Arch_invokeIssueSGISignal(irq, target, destSlot, srcSlot);

#पूर्ण_अगर
#अगर_घोषित ENABLE_SMP_SUPPORT
    } अन्यथा अगर (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        word_t irq_w = getSyscallArg(0, बफर);
        bool_t trigger = !!getSyscallArg(1, बफर);
        word_t सूचकांक = getSyscallArg(2, बफर);
        word_t depth = getSyscallArg(3, बफर) & 0xfful;
        seL4_Word target = getSyscallArg(4, बफर);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t status = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_TO_IRQT(target, irq_w);

        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        अगर (target >= CONFIG_MAX_अंक_NODES) {
            current_syscall_error.type = seL4_InvalidArgument;
            userError("Target core %lu is invalid.", target);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(बफर, 0), (पूर्णांक)IRQT_TO_IRQ(irq));
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        अगर (!IRQ_IS_PPI(irq)) {
            setIRQTarget(irq, target);
        }
        वापस Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
    } अन्यथा {
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT

अटल exception_t invokeSGISignalGenerate(word_t irq, word_t target)
{
    plat_sendSGI(irq, target);
    वापस EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(word_t invLabel, word_t लंबाई,
                                      cap_t cap, word_t *बफर)
{

    word_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    word_t target = cap_sgi_signal_cap_get_capSGITarget(cap);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSGISignalGenerate(irq, target);
}
#पूर्ण_अगर /* !CONFIG_ENABLE_SMP_SUPPORT */
