/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <config.h>

#ضمن <arch/object/interrupt.h>

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خونه, cte_t *control_خونه, bool_t trigger)
{
    إذا (config_سېټ(HAVE_سېټ_TRIGGER)) {
        سېټ_IRQTrigger(irq, trigger);
    }
    أرجع invokeIRQControl(irq, handler_خونه, control_خونه);
}


#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t Arch_invokeIssueSGISignal(کلمه_t irq, کلمه_t موخه, cte_t *sgi_خونه, cte_t *control_خونه)
{
    cte_ورننباسه(cap_sgi_signal_cap_new(موخه, irq), control_خونه, sgi_خونه);
    أرجع EXCEPTION_NONE;
}

#د_که_پای

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                            cte_t *src_خونه, کلمه_t *بفر)
{
    إذا (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        إذا (اوږدتیا < 4 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!config_سېټ(HAVE_سېټ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t irq_w = getSyscallArg(0, بفر);
        irq_t irq = (irq_t) CORE_IRQ_منزل_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, بفر);
        کلمه_t شاخص = getSyscallArg(2, بفر);
        کلمه_t ژوروالی = getSyscallArg(3, بفر);

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
#د_که_پای
        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خونه_ret_t lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_خونه, src_خونه, trigger);
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueSGISignal) {
        إذا (اوږدتیا < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (wrap_config_سېټ(عدد_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t irq = getSyscallArg(0, بفر);
        کلمه_t موخه = getSyscallArg(1, بفر);
        کلمه_t شاخص = getSyscallArg(2, بفر);
        کلمه_t ژوروالی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (irq >= wrap_config_سېټ(عدد_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = عدد_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!plat_SGITargetValid(موخه)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", موخه);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خونه_ret_t lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع lu_ret.حالت;
        }
        cte_t *dest_خونه = lu_ret.خونه;

        exception_t حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع حالت;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIssueSGISignal(irq, موخه, dest_خونه, src_خونه);

#د_که_پای
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        کلمه_t irq_w = getSyscallArg(0, بفر);
        bool_t trigger = !!getSyscallArg(1, بفر);
        کلمه_t شاخص = getSyscallArg(2, بفر);
        کلمه_t ژوروالی = getSyscallArg(3, بفر) & 0xfful;
        se_L4_کلمه موخه = getSyscallArg(4, بفر);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t حالت = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_منزل_IRQT(موخه, irq_w);

        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (موخه >= CONFIG_MAX_عدد_NODES) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            userError("Target core %lu is invalid.", موخه);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خونه_ret_t lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        إذا (!IRQ_IS_PPI(irq)) {
            سېټ_IRQTarget(irq, موخه);
        }
        أرجع Arch_invokeIRQControl(irq, dest_خونه, src_خونه, trigger);
#د_که_پای /* ENABLE_SMP_SUPPORT */
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t invokeSGISignalGenerate(کلمه_t irq, کلمه_t موخه)
{
    plat_sendSGI(irq, موخه);
    أرجع EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                      cap_t cap, کلمه_t *بفر)
{

    کلمه_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    کلمه_t موخه = cap_sgi_signal_cap_get_capSGITarget(cap);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSGISignalGenerate(irq, موخه);
}
#د_که_پای /* !CONFIG_ENABLE_SMP_SUPPORT */
