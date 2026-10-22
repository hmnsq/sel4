/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>
#شامل <config.h>

#شامل <arch/object/interrupt.h>

ساکن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خانہ, cte_t *control_خانہ, bool_t trigger)
{
    اگر (config_سیٹ(HAVE_سیٹ_TRIGGER)) {
        سیٹ_IRQTrigger(irq, trigger);
    }
    واپس invokeIRQControl(irq, handler_خانہ, control_خانہ);
}


#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT

ساکن exception_t Arch_invokeIssueSGISignal(لفظ_t irq, لفظ_t ہدف, cte_t *sgi_خانہ, cte_t *control_خانہ)
{
    cte_داخل_کرو(cap_sgi_signal_cap_new(ہدف, irq), control_خانہ, sgi_خانہ);
    واپس EXCEPTION_NONE;
}

#اگر_ختم

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                            cte_t *src_خانہ, لفظ_t *بفر)
{
    اگر (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        اگر (لمبائی < 4 || current_extra_caps.excaprefs[0] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (!config_سیٹ(HAVE_سیٹ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq_w = getSyscallArg(0, بفر);
        irq_t irq = (irq_t) CORE_IRQ_مقصد_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t گہرائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت_2 = Arch_checkIRQ(irq_w);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

#اگر defined ENABLE_SMP_SUPPORT
        اگر (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم
        اگر (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحیح)IRQT_مقصد_IRQ(irq));
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خانہ_ret_t lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس lu_ret.حالت_2;
        }

        cte_t *dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس Arch_invokeIRQControl(irq, dest_خانہ, src_خانہ, trigger);
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    } ورنہ اگر (invLabel == ARMIRQIssueSGISignal) {
        اگر (لمبائی < 4 || current_extra_caps.excaprefs[0] == تہی) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        اگر (wrap_config_سیٹ(عدد_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq = getSyscallArg(0, بفر);
        لفظ_t ہدف = getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t گہرائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        اگر (irq >= wrap_config_سیٹ(عدد_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = عدد_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (!plat_SGITargetValid(ہدف)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", ہدف);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خانہ_ret_t lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(بفر, 0));
            واپس lu_ret.حالت_2;
        }
        cte_t *dest_خانہ = lu_ret.خانہ;

        exception_t حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(بفر, 0));
            واپس حالت_2;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس Arch_invokeIssueSGISignal(irq, ہدف, dest_خانہ, src_خانہ);

#اگر_ختم
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    } ورنہ اگر (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        لفظ_t irq_w = getSyscallArg(0, بفر);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t گہرائی = getSyscallArg(3, بفر) & 0xfful;
        se_L4_لفظ ہدف = getSyscallArg(4, بفر);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t حالت_2 = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_مقصد_IRQT(ہدف, irq_w);

        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        اگر (ہدف >= CONFIG_MAX_عدد_NODES) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            userError("Target core %lu is invalid.", ہدف);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحیح)IRQT_مقصد_IRQ(irq));
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خانہ_ret_t lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس lu_ret.حالت_2;
        }

        cte_t *dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)IRQT_مقصد_IRQ(irq));
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        اگر (!IRQ_IS_PPI(irq)) {
            سیٹ_IRQTarget(irq, ہدف);
        }
        واپس Arch_invokeIRQControl(irq, dest_خانہ, src_خانہ, trigger);
#اگر_ختم /* ENABLE_SMP_SUPPORT */
    } ورنہ {
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT

ساکن exception_t invokeSGISignalGenerate(لفظ_t irq, لفظ_t ہدف)
{
    plat_sendSGI(irq, ہدف);
    واپس EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                      cap_t cap, لفظ_t *بفر)
{

    لفظ_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    لفظ_t ہدف = cap_sgi_signal_cap_get_capSGITarget(cap);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeSGISignalGenerate(irq, ہدف);
}
#اگر_ختم /* !CONFIG_ENABLE_SMP_SUPPORT */
