/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <config.h>

#ಅನ್ತರ್ಭಾವ <arch/object/interrupt.h>

ಸ್ಥಿರ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ, bool_t trigger)
{
    ಆದರೆ (config_ಗಣ(HAVE_ಗಣ_TRIGGER)) {
        ಗಣ_IRQTrigger(irq, trigger);
    }
    ಹಿಂತಿರುಗಿ invokeIRQControl(irq, handler_ಸ್ಥಾನ, control_ಸ್ಥಾನ);
}


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT

ಸ್ಥಿರ exception_t Arch_invokeIssueSGISignal(ಶಬ್ದ_t irq, ಶಬ್ದ_t ಗುರಿ, cte_t *sgi_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ)
{
    cte_ಸೇರಿಸು(cap_sgi_signal_cap_new(ಗುರಿ, irq), control_ಸ್ಥಾನ, sgi_ಸ್ಥಾನ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ಆದರೆ_ಅಂತ್ಯ

exception_t Arch_decodeIRQControlInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                            cte_t *src_ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        ಆದರೆ (ಉದ್ದಳತೆ < 4 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (!config_ಗಣ(HAVE_ಗಣ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಶಬ್ದ_t irq_w = getSyscallArg(0, ಬಫರ್);
        irq_t irq = (irq_t) CORE_IRQ_ಗಮ್ಯ_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, ಬಫರ್);
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t ಆಳ = getSyscallArg(3, ಬಫರ್);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t ಸ್ಥಿತಿ_2 = Arch_checkIRQ(irq_w);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

#ಆದರೆ defined ENABLE_SMP_SUPPORT
        ಆದರೆ (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ಸ್ಥಾನ_ret_t lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }

        cte_t *dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ Arch_invokeIRQControl(irq, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ, trigger);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (invLabel == ARMIRQIssueSGISignal) {
        ಆದರೆ (ಉದ್ದಳತೆ < 4 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ಆದರೆ (wrap_config_ಗಣ(ಅಂಕೆ_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಶಬ್ದ_t irq = getSyscallArg(0, ಬಫರ್);
        ಶಬ್ದ_t ಗುರಿ = getSyscallArg(1, ಬಫರ್);
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t ಆಳ = getSyscallArg(3, ಬಫರ್);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ಆದರೆ (irq >= wrap_config_ಗಣ(ಅಂಕೆ_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = ಅಂಕೆ_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (!plat_SGITargetValid(ಗುರಿ)) {
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", ಗುರಿ);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ಸ್ಥಾನ_ret_t lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(ಬಫರ್, 0));
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        cte_t *dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        exception_t ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(ಬಫರ್, 0));
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ Arch_invokeIssueSGISignal(irq, ಗುರಿ, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ);

#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        ಶಬ್ದ_t irq_w = getSyscallArg(0, ಬಫರ್);
        bool_t trigger = !!getSyscallArg(1, ಬಫರ್);
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t ಆಳ = getSyscallArg(3, ಬಫರ್) & 0xfful;
        se_L4_ಶಬ್ದ ಗುರಿ = getSyscallArg(4, ಬಫರ್);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t ಸ್ಥಿತಿ_2 = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_ಗಮ್ಯ_IRQT(ಗುರಿ, irq_w);

        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಆದರೆ (ಗುರಿ >= CONFIG_MAX_ಅಂಕೆ_NODES) {
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            userError("Target core %lu is invalid.", ಗುರಿ);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ಸ್ಥಾನ_ret_t lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }

        cte_t *dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_IRQ(irq));
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        ಆದರೆ (!IRQ_IS_PPI(irq)) {
            ಗಣ_IRQTarget(irq, ಗುರಿ);
        }
        ಹಿಂತಿರುಗಿ Arch_invokeIRQControl(irq, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ, trigger);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
    } ಇಲ್ಲದಿದ್ದರೆ {
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT

ಸ್ಥಿರ exception_t invokeSGISignalGenerate(ಶಬ್ದ_t irq, ಶಬ್ದ_t ಗುರಿ)
{
    plat_sendSGI(irq, ಗುರಿ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                      cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{

    ಶಬ್ದ_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    ಶಬ್ದ_t ಗುರಿ = cap_sgi_signal_cap_get_capSGITarget(cap);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeSGISignalGenerate(irq, ಗುರಿ);
}
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_ENABLE_SMP_SUPPORT */
