/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/interrupt.h>

ਸਥਿਰ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ਥਾਂ, cte_t *control_ਥਾਂ, bool_t trigger)
{
    ਜੇ (config_ਸੈੱਟ(HAVE_ਸੈੱਟ_TRIGGER)) {
        ਸੈੱਟ_IRQTrigger(irq, trigger);
    }
    ਵਾਪਸ invokeIRQControl(irq, handler_ਥਾਂ, control_ਥਾਂ);
}


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT

ਸਥਿਰ exception_t Arch_invokeIssueSGISignal(ਸ਼ਬਦ_t irq, ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ, cte_t *sgi_ਥਾਂ, cte_t *control_ਥਾਂ)
{
    cte_ਪਾਓ(cap_sgi_signal_cap_new(ਨਿਸ਼ਾਨਾ, irq), control_ਥਾਂ, sgi_ਥਾਂ);
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਅੰਤ

exception_t Arch_decodeIRQControlInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                            cte_t *src_ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (invLabel == ARMIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, ਬਫਰ);
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t ਸਥਿਤੀ = Arch_checkIRQ(irq_w);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

#ਜੇ defined ENABLE_SMP_SUPPORT
        ਜੇ (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ
        ਜੇ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("Rejecting request for IRQ %u. Already active.", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ਥਾਂ_ret_t lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }

        cte_t *dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ Arch_invokeIRQControl(irq, dest_ਥਾਂ, src_ਥਾਂ, trigger);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    } ਨਹੀਂਤਾਂ ਜੇ (invLabel == ARMIRQIssueSGISignal) {
        ਜੇ (ਲੰਮਾਈ < 4 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ਜੇ (wrap_config_ਸੈੱਟ(ਅੰਕ_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ਬਦ_t irq = getSyscallArg(0, ਬਫਰ);
        ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ = getSyscallArg(1, ਬਫਰ);
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        ਜੇ (irq >= wrap_config_ਸੈੱਟ(ਅੰਕ_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = ਅੰਕ_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (!plat_SGITargetValid(ਨਿਸ਼ਾਨਾ)) {
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", ਨਿਸ਼ਾਨਾ);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ਥਾਂ_ret_t lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(ਬਫਰ, 0));
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        cte_t *dest_ਥਾਂ = lu_ret.ਥਾਂ;

        exception_t ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(ਬਫਰ, 0));
            ਵਾਪਸ ਸਥਿਤੀ;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ Arch_invokeIssueSGISignal(irq, ਨਿਸ਼ਾਨਾ, dest_ਥਾਂ, src_ਥਾਂ);

#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    } ਨਹੀਂਤਾਂ ਜੇ (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        ਸ਼ਬਦ_t irq_w = getSyscallArg(0, ਬਫਰ);
        bool_t trigger = !!getSyscallArg(1, ਬਫਰ);
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ) & 0xfful;
        se_L4_ਸ਼ਬਦ ਨਿਸ਼ਾਨਾ = getSyscallArg(4, ਬਫਰ);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t ਸਥਿਤੀ = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(ਨਿਸ਼ਾਨਾ, irq_w);

        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਜੇ (ਨਿਸ਼ਾਨਾ >= CONFIG_MAX_ਅੰਕ_NODES) {
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            userError("Target core %lu is invalid.", ਨਿਸ਼ਾਨਾ);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("Rejecting request for IRQ %u. Already active.", (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        lookup_ਥਾਂ_ret_t lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }

        cte_t *dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        ਜੇ (!IRQ_IS_PPI(irq)) {
            ਸੈੱਟ_IRQTarget(irq, ਨਿਸ਼ਾਨਾ);
        }
        ਵਾਪਸ Arch_invokeIRQControl(irq, dest_ਥਾਂ, src_ਥਾਂ, trigger);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
    } ਨਹੀਂਤਾਂ {
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT

ਸਥਿਰ exception_t invokeSGISignalGenerate(ਸ਼ਬਦ_t irq, ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    plat_sendSGI(irq, ਨਿਸ਼ਾਨਾ);
    ਵਾਪਸ EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                      cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{

    ਸ਼ਬਦ_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ = cap_sgi_signal_cap_get_capSGITarget(cap);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeSGISignalGenerate(irq, ਨਿਸ਼ਾਨਾ);
}
#ਜੇ_ਅੰਤ /* !CONFIG_ENABLE_SMP_SUPPORT */
