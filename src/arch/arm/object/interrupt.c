/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <config.h>

#ଅନ୍ତର୍ଭାଵ <arch/object/interrupt.h>

ସ୍ଥିର exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
    ଯଦି (config_set(HAVE_SET_TRIGGER)) {
        setIRQTrigger(irq, trigger);
    }
    ଫେରନ୍ତୁ invokeIRQControl(irq, handlerSlot, controlSlot);
}


#ifndef CONFIG_ENABLE_SMP_SUPPORT

ସ୍ଥିର exception_t Arch_invokeIssueSGISignal(word_t irq, word_t target, cte_t *sgiSlot, cte_t *controlSlot)
{
    cte_ଭର୍ତ୍ତି(cap_sgi_signal_cap_new(target, irq), controlSlot, sgiSlot);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#endif

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    ଯଦି (invLabel == ARMIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

#ଯଦି defined ENABLE_SMP_SUPPORT
        ଯଦି (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ଯଦି (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ status;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    } ଅନ୍ୟଥା ଯଦି (invLabel == ARMIRQIssueSGISignal) {
        ଯଦି (length < 4 || current_extra_caps.excaprefs[0] == ଖାଲି) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ଯଦି (wrap_config_set(ଅଙ୍କ_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq = getSyscallArg(0, buffer);
        word_t target = getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ଯଦି (irq >= wrap_config_set(ଅଙ୍କ_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = ଅଙ୍କ_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (!plat_SGITargetValid(target)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", target);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(buffer, 0));
            ଫେରନ୍ତୁ lu_ret.status;
        }
        cte_t *destSlot = lu_ret.slot;

        exception_t status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(buffer, 0));
            ଫେରନ୍ତୁ status;
        }
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ Arch_invokeIssueSGISignal(irq, target, destSlot, srcSlot);

#endif
#ifdef ENABLE_SMP_SUPPORT
    } ଅନ୍ୟଥା ଯଦି (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        word_t irq_w = getSyscallArg(0, buffer);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer) & 0xfful;
        seL4_Word target = getSyscallArg(4, buffer);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t status = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_TO_IRQT(target, irq_w);

        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

        ଯଦି (target >= CONFIG_MAX_ଅଙ୍କ_NODES) {
            current_syscall_error.type = seL4_InvalidArgument;
            userError("Target core %lu is invalid.", target);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)IRQT_TO_IRQ(irq));
            ଫେରନ୍ତୁ status;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        ଯଦି (!IRQ_IS_PPI(irq)) {
            setIRQTarget(irq, target);
        }
        ଫେରନ୍ତୁ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#endif /* ENABLE_SMP_SUPPORT */
    } ଅନ୍ୟଥା {
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

#ifndef CONFIG_ENABLE_SMP_SUPPORT

ସ୍ଥିର exception_t invokeSGISignalGenerate(word_t irq, word_t target)
{
    plat_sendSGI(irq, target);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(word_t invLabel, word_t length,
                                      cap_t cap, word_t *buffer)
{

    word_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    word_t target = cap_sgi_signal_cap_get_capSGITarget(cap);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSGISignalGenerate(irq, target);
}
#endif /* !CONFIG_ENABLE_SMP_SUPPORT */
