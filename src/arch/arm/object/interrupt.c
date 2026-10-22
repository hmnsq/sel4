/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <config.h>

#ضمن <arch/object/interrupt.h>

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_جڳهه, cte_t *control_جڳهه, bool_t trigger)
{
    إذا (config_سيٽ(HAVE_سيٽ_TRIGGER)) {
        سيٽ_IRQTrigger(irq, trigger);
    }
    أرجع invokeIRQControl(irq, handler_جڳهه, control_جڳهه);
}


#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t Arch_invokeIssueSGISignal(لفظ_t irq, لفظ_t هدف, cte_t *sgi_جڳهه, cte_t *control_جڳهه)
{
    cte_وجهو(cap_sgi_signal_cap_new(هدف, irq), control_جڳهه, sgi_جڳهه);
    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_پورو

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                            cte_t *src_جڳهه, لفظ_t *بفر)
{
    إذا (invLabel == ARMIRQIssueIRQHandlerTrigger) {
        إذا (ڊيگهائي < 4 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!config_سيٽ(HAVE_سيٽ_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq_w = getSyscallArg(0, بفر);
        irq_t irq = (irq_t) CORE_IRQ_منزل_IRQT(0, irq_w);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاريو = getSyscallArg(2, بفر);
        لفظ_t اونهائي = getSyscallArg(3, بفر);

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
#جيڪڏهن_پورو
        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جڳهه_ret_t lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_جڳهه, src_جڳهه, trigger);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueSGISignal) {
        إذا (ڊيگهائي < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("IRQControl: IssueSGISignal: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (wrap_config_سيٽ(انگ_SGIS) == 0) {
            current_syscall_error.type = seL4_IllegalOperation;
            userError("IRQControl: IssueSGISignal not available on this platform.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t irq = getSyscallArg(0, بفر);
        لفظ_t هدف = getSyscallArg(1, بفر);
        لفظ_t اشاريو = getSyscallArg(2, بفر);
        لفظ_t اونهائي = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        /* wrap_config_set to prevent verification automation from simplifying
           away the condition when NUM_SGIS = 0 */
        إذا (irq >= wrap_config_سيٽ(انگ_SGIS)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = انگ_SGIS - 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI IRQ 0x%lx.", irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (!plat_SGITargetValid(هدف)) {
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 1;
            userError("IRQControl: IssueSGISignal: Invalid SGI Target 0x%lx.", هدف);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جڳهه_ret_t lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap invalid: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع lu_ret.حالت;
        }
        cte_t *dest_جڳهه = lu_ret.جڳهه;

        exception_t حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("IRQControl: IssueSGISignal: Target slot for new ARM_SGI_Signal cap not empty: cap %lu.",
                      getExtraCPtr(بفر, 0));
            أرجع حالت;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIssueSGISignal(irq, هدف, dest_جڳهه, src_جڳهه);

#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    } وإلا إذا (invLabel == ARMIRQIssueIRQHandlerTriggerCore) {
        لفظ_t irq_w = getSyscallArg(0, بفر);
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاريو = getSyscallArg(2, بفر);
        لفظ_t اونهائي = getSyscallArg(3, بفر) & 0xfful;
        se_L4_لفظ هدف = getSyscallArg(4, بفر);
        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;
        exception_t حالت = Arch_checkIRQ(irq_w);
        irq_t irq = CORE_IRQ_منزل_IRQT(هدف, irq_w);

        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (هدف >= CONFIG_MAX_انگ_NODES) {
            current_syscall_error.type = se_L4_Invalid_دليل;
            userError("Target core %lu is invalid.", هدف);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)IRQT_منزل_IRQ(irq));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جڳهه_ret_t lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع lu_ret.حالت;
        }

        cte_t *dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)IRQT_منزل_IRQ(irq));
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);

        /* If the IRQ is not a private interrupt, then the role of the syscall is to set
         * target core to which the shared interrupt will be physically delivered.
         */
        إذا (!IRQ_IS_PPI(irq)) {
            سيٽ_IRQTarget(irq, هدف);
        }
        أرجع Arch_invokeIRQControl(irq, dest_جڳهه, src_جڳهه, trigger);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT

ساكن exception_t invokeSGISignalGenerate(لفظ_t irq, لفظ_t هدف)
{
    plat_sendSGI(irq, هدف);
    أرجع EXCEPTION_NONE;
}


exception_t decodeSGISignalInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                      cap_t cap, لفظ_t *بفر)
{

    لفظ_t irq = cap_sgi_signal_cap_get_capSGIIRQ(cap);
    لفظ_t هدف = cap_sgi_signal_cap_get_capSGITarget(cap);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSGISignalGenerate(irq, هدف);
}
#جيڪڏهن_پورو /* !CONFIG_ENABLE_SMP_SUPPORT */
