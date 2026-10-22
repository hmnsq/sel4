/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <config.h>

#ضمن <arch/object/interrupt.h>

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_جایگاه, cte_t *control_جایگاه, bool_t trigger)
{
    إذا (config_مجموعه(HAVE_مجموعه_TRIGGER)) {
        مجموعه_IRQTrigger(irq, trigger);
    }
    أرجع invokeIRQControl(irq, handler_جایگاه, control_جایگاه);
}


#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t Arch_invokeIssueSGISignal(کلمه_t irq, کلمه_t هدف, cte_t *sgi_جایگاه, cte_t *control_جایگاه)
{
    cte_درج(cap_sgi_signal_cap_new(هدف, irq), control_جایگاه, sgi_جایگاه);
    أرجع EXCEPTION_NONE;
}

#پایان_اگر

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t درازی,
                                            cte_t *src_جایگاه, کلمه_t *میانگیر)
{
    إذا (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        إذا (درازی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!config_مجموعه(HAVE_مجموعه_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t irq_w = getSyscallArg(0, میانگیر);
        irq_t irq = (irq_t) CORE_IRQ_مقصد_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, میانگیر);
        کلمه_t شاخص = getSyscallArg(2, میانگیر);
        کلمه_t عمق = getSyscallArg(3, میانگیر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t وضعیت = Arch_checkIRQ(irq_w);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

#إذا defined ENABLE_SMP_SUPPORT
        إذا (IRQ_IS_PPI(irq)) {
            userError("Trying to get a handler on a PPI: use GetTriggerCore.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر
        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جایگاه_ret_t lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع lu_ret.وضعیت;
        }

        cte_t *dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_جایگاه, src_جایگاه, trigger);
#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueSGISignal) {
        إذا (درازی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (wrap_config_مجموعه(رقم_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t irq = getSyscallArg(0, میانگیر);
        کلمه_t هدف = getSyscallArg(1, میانگیر);
        کلمه_t شاخص = getSyscallArg(2, میانگیر);
        کلمه_t عمق = getSyscallArg(3, میانگیر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (irq >= wrap_config_مجموعه(رقم_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = رقم_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!plat_SGITargetValid(هدف)) {
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", هدف);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جایگاه_ret_t lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(میانگیر, 0));
            أرجع lu_ret.وضعیت;
        }
        cte_t *dest_جایگاه = lu_ret.جایگاه;

        exception_t وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(میانگیر, 0));
            أرجع وضعیت;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIssueSGISignal(irq, هدف, dest_جایگاه, src_جایگاه);

#پایان_اگر
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        کلمه_t irq_w = getSyscallArg(0, میانگیر);
        bool_t trigger = !!getSyscallArg(1, میانگیر);
        کلمه_t شاخص = getSyscallArg(2, میانگیر);
        کلمه_t عمق = getSyscallArg(3, میانگیر) & 0xfful;
        se_L4_کلمه هدف = getSyscallArg(4, میانگیر);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t وضعیت = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_مقصد_IRQT(هدف, irq_w);

        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        إذا (هدف >= CONFIG_MAX_رقم_NODES) {
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            userError("Target core %lu is invalid.", هدف);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جایگاه_ret_t lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع lu_ret.وضعیت;
        }

        cte_t *dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)IRQT_مقصد_IRQ(irq));
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        إذا (!IRQ_IS_PPI(irq)) {
            مجموعه_IRQTarget(irq, هدف);
        }
        أرجع Arch_invokeIRQControl(irq, dest_جایگاه, src_جایگاه, trigger);
#پایان_اگر /* ENABLE_SMP_SUPPORT */
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t invokeSGISignalGenerate(کلمه_t irq, کلمه_t هدف)
{
    plat_sendSGI(irq, هدف);
    أرجع EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(کلمه_t invLabel, کلمه_t درازی,
                                      cap_t cap, کلمه_t *میانگیر)
{

    کلمه_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    کلمه_t هدف = cap_sgi_signal_cap_get_capSGITarget(cap);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSGISignalGenerate(irq, هدف);
}
#پایان_اگر /* !CONFIG_ENABLE_SMP_SUPPORT */
