/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <config.h>

#অন্তর্ভাব <arch/object/interrupt.h>

স্থির exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
    যদি (config_set(HAVE_SET_TRIGGER)) {
        setIRQTrigger(irq, trigger);
    }
    প্রত্যাগম invokeIRQControl(irq, handlerSlot, controlSlot);
}


#ifndef CONFIG_ENABLE_SMP_SUPPORT

স্থির exception_t Arch_invokeIssueSGISignal(word_t irq, word_t target, cte_t *sgiSlot, cte_t *controlSlot)
{
    cte_ঢোকাও(cap_sgi_signal_cap_new(target, irq), controlSlot, sgiSlot);
    প্রত্যাগম EXCEPTION_NONE;
}

#endif

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    যদি (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        যদি (length < 4 || current_extra_caps.excaprefs[0] == রিক্ত) {
            current_syscall_error.type = seL4_TruncatedMessage;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        যদি (status != EXCEPTION_NONE) {
            প্রত্যাগম status;
        }

#যদি defined ENABLE_SMP_SUPPORT
        যদি (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
#endif
        যদি (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    } অন্যথা যদি (invLabel == ARMIRQIssueSGISignal) {
        যদি (length < 4 || current_extra_caps.excaprefs[0] == রিক্ত) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        যদি (wrap_config_set(অঙ্ক_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq = getSyscallArg(0, buffer);
        word_t target = getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        যদি (irq >= wrap_config_set(অঙ্ক_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = অঙ্ক_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (!plat_SGITargetValid(target)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", target);
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(buffer, 0));
            প্রত্যাগম lu_ret.status;
        }
        cte_t *destSlot = lu_ret.slot;

        exception_t status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(buffer, 0));
            প্রত্যাগম status;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম Arch_invokeIssueSGISignal(irq, target, destSlot, srcSlot);

#endif
#ifdef ENABLE_SMP_SUPPORT
    } অন্যথা যদি (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        word_t irq_w = getSyscallArg(0, buffer);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer) & 0xfful;
        seL4_Word target = getSyscallArg(4, buffer);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t status = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_TO_IRQT(target, irq_w);

        যদি (status != EXCEPTION_NONE) {
            প্রত্যাগম status;
        }

        যদি (target >= CONFIG_MAX_অঙ্ক_NODES) {
            current_syscall_error.type = seL4_InvalidArgument;
            userError("Target core %lu is invalid.", target);
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (পূর্ণাঙ্ক)IRQT_TO_IRQ(irq));
            প্রত্যাগম status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        যদি (!IRQ_IS_PPI(irq)) {
            setIRQTarget(irq, target);
        }
        প্রত্যাগম Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#endif /* ENABLE_SMP_SUPPORT */
    } অন্যথা {
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
}

#ifndef CONFIG_ENABLE_SMP_SUPPORT

স্থির exception_t invokeSGISignalGenerate(word_t irq, word_t target)
{
    plat_sendSGI(irq, target);
    প্রত্যাগম EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(word_t invLabel, word_t length,
                                      cap_t cap, word_t *buffer)
{

    word_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    word_t target = cap_sgi_signal_cap_get_capSGITarget(cap);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSGISignalGenerate(irq, target);
}
#endif /* !CONFIG_ENABLE_SMP_SUPPORT */
