/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <config.h>

#ضمن <arch/object/interrupt.h>

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_تھاں, cte_t *control_تھاں, bool_t trigger)
{
    إذا (config_سیٹ(HAVE_سیٹ_TRIGGER)) {
        سیٹ_IRQTrigger(irq, trigger);
    }
    أرجع invokeIRQControl(irq, handler_تھاں, control_تھاں);
}


#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t Arch_invokeIssueSGISignal(لفظ_t irq, لفظ_t نشانہ, cte_t *sgi_تھاں, cte_t *control_تھاں)
{
    cte_داخل_کرو(cap_sgi_signal_cap_new(نشانہ, irq), control_تھاں, sgi_تھاں);
    أرجع EXCEPTION_NONE;
}

#جے_انت

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                            cte_t *src_تھاں, لفظ_t *بفر)
{
    إذا (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        إذا (لمائی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!config_سیٹ(HAVE_سیٹ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq_w = getSyscallArg(0, بفر);
        irq_t irq = (irq_t) CORE_IRQ_منزل_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t ڈونگھائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

#إذا defined ENABLE_SMP_SUPPORT
        إذا (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت
        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_تھاں_ret_t lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_تھاں, src_تھاں, trigger);
#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueSGISignal) {
        إذا (لمائی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (wrap_config_سیٹ(عدد_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq = getSyscallArg(0, بفر);
        لفظ_t نشانہ = getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t ڈونگھائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (irq >= wrap_config_سیٹ(عدد_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = عدد_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!plat_SGITargetValid(نشانہ)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", نشانہ);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_تھاں_ret_t lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع lu_ret.حالت;
        }
        cte_t *dest_تھاں = lu_ret.تھاں;

        exception_t حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع حالت;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIssueSGISignal(irq, نشانہ, dest_تھاں, src_تھاں);

#جے_انت
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        لفظ_t irq_w = getSyscallArg(0, بفر);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t ڈونگھائی = getSyscallArg(3, بفر) & 0xfful;
        se_L4_لفظ نشانہ = getSyscallArg(4, بفر);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t حالت = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_منزل_IRQT(نشانہ, irq_w);

        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (نشانہ >= CONFIG_MAX_عدد_NODES) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            userError("Target core %lu is invalid.", نشانہ);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_تھاں_ret_t lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        إذا (!IRQ_IS_PPI(irq)) {
            سیٹ_IRQTarget(irq, نشانہ);
        }
        أرجع Arch_invokeIRQControl(irq, dest_تھاں, src_تھاں, trigger);
#جے_انت /* ENABLE_SMP_SUPPORT */
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t invokeSGISignalGenerate(لفظ_t irq, لفظ_t نشانہ)
{
    plat_sendSGI(irq, نشانہ);
    أرجع EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                      cap_t cap, لفظ_t *بفر)
{

    لفظ_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    لفظ_t نشانہ = cap_sgi_signal_cap_get_capSGITarget(cap);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSGISignalGenerate(irq, نشانہ);
}
#جے_انت /* !CONFIG_ENABLE_SMP_SUPPORT */
