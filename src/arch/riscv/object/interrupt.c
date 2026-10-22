/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/interrupt.h>

exception_t Arch_checkIRQ(ਸ਼ਬਦ_t irq)
{
    ਜੇ (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (ਪੂਰਨ)irq, maxIRQ);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ਥਾਂ, cte_t *control_ਥਾਂ, bool_t trigger)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਸੈੱਟ_TRIGGER
    ਸੈੱਟ_IRQTrigger(irq, trigger);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ invokeIRQControl(irq, handler_ਥਾਂ, control_ਥਾਂ);
}

exception_t Arch_decodeIRQControlInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                            cte_t *src_ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        ਜੇ (ਲੰਮਾਈ < 4 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (!config_ਸੈੱਟ(HAVE_ਸੈੱਟ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ਬਦ_t irq_w = getSyscallArg(0, ਬਫਰ);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, ਬਫਰ);
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t ਸਥਿਤੀ = Arch_checkIRQ(irq_w);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਜੇ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("Rejecting request for IRQ %u. Already active.", (ਪੂਰਨ)irq);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ਥਾਂ_ret_t lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)irq);
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }

        cte_t *dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)irq);
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ Arch_invokeIRQControl(irq, dest_ਥਾਂ, src_ਥਾਂ, trigger);
    } ਨਹੀਂਤਾਂ {
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}
