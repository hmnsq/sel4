/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <config.h>

#ಅನ್ತರ್ಭಾವ <arch/object/interrupt.h>

ಸ್ಥಿರ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
    ಯದಿ (config_set(HAVE_SET_TRIGGER)) {
        setIRQTrigger(irq, trigger);
    }
    ಪ್ರತ್ಯಾಗಮ invokeIRQControl(irq, handlerSlot, controlSlot);
}


#ifndef CONFIG_ENABLE_SMP_SUPPORT

ಸ್ಥಿರ exception_t Arch_invokeIssueSGISignal(word_t irq, word_t target, cte_t *sgiSlot, cte_t *controlSlot)
{
    cte_ಸೇರಿಸು(cap_sgi_signal_cap_new(target, irq), controlSlot, sgiSlot);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#endif

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    ಯದಿ (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        ಯದಿ (length < 4 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

#ಯದಿ defined ENABLE_SMP_SUPPORT
        ಯದಿ (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ಯದಿ (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    } ಅನ್ಯಥಾ ಯದಿ (invLabel == ARMIRQIssueSGISignal) {
        ಯದಿ (length < 4 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ಯದಿ (wrap_config_set(ಅಂಕೆ_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq = getSyscallArg(0, buffer);
        word_t target = getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ಯದಿ (irq >= wrap_config_set(ಅಂಕೆ_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = ಅಂಕೆ_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (!plat_SGITargetValid(target)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", target);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(buffer, 0));
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        cte_t *destSlot = lu_ret.slot;

        exception_t status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(buffer, 0));
            ಪ್ರತ್ಯಾಗಮ status;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ Arch_invokeIssueSGISignal(irq, target, destSlot, srcSlot);

#endif
#ifdef ENABLE_SMP_SUPPORT
    } ಅನ್ಯಥಾ ಯದಿ (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        word_t irq_w = getSyscallArg(0, buffer);
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer) & 0xfful;
        seL4_Word target = getSyscallArg(4, buffer);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t status = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_TO_IRQT(target, irq_w);

        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        ಯದಿ (target >= CONFIG_MAX_ಅಂಕೆ_NODES) {
            current_syscall_error.type = seL4_InvalidArgument;
            userError("Target core %lu is invalid.", target);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_IRQ(irq));
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        ಯದಿ (!IRQ_IS_PPI(irq)) {
            setIRQTarget(irq, target);
        }
        ಪ್ರತ್ಯಾಗಮ Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
#endif /* ENABLE_SMP_SUPPORT */
    } ಅನ್ಯಥಾ {
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

#ifndef CONFIG_ENABLE_SMP_SUPPORT

ಸ್ಥಿರ exception_t invokeSGISignalGenerate(word_t irq, word_t target)
{
    plat_sendSGI(irq, target);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(word_t invLabel, word_t length,
                                      cap_t cap, word_t *buffer)
{

    word_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    word_t target = cap_sgi_signal_cap_get_capSGITarget(cap);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSGISignalGenerate(irq, target);
}
#endif /* !CONFIG_ENABLE_SMP_SUPPORT */
