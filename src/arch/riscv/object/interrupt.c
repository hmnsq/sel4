/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>

#ಅನ್ತರ್ಭಾವ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(ಶಬ್ದ_t irq)
{
    ಆದರೆ (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (ಪೂರ್ಣಾಂಕ)irq, maxIRQ);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ, bool_t trigger)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಗಣ_TRIGGER
    ಗಣ_IRQTrigger(irq, trigger);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ invokeIRQControl(irq, handler_ಸ್ಥಾನ, control_ಸ್ಥಾನ);
}

exception_t Arch_decodeIRQControlInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                            cte_t *src_ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, ಬಫರ್);
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t ಆಳ = getSyscallArg(3, ಬಫರ್);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t ಸ್ಥಿತಿ_2 = Arch_checkIRQ(irq_w);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಆದರೆ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("Rejecting request for IRQ %u. Already active.", (ಪೂರ್ಣಾಂಕ)irq);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ಸ್ಥಾನ_ret_t lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)irq);
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }

        cte_t *dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)irq);
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ Arch_invokeIRQControl(irq, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ, trigger);
    } ಇಲ್ಲದಿದ್ದರೆ {
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}
