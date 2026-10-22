/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>

#ضمن <arch/object/interrupt.h>

exception_t Arch_checkIRQ(کلمه_t irq)
{
    إذا (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (صحيح)irq, maxIRQ);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خونه, cte_t *control_خونه, bool_t trigger)
{
#که_تعریف_شوی HAVE_سېټ_TRIGGER
    سېټ_IRQTrigger(irq, trigger);
#د_که_پای
    أرجع invokeIRQControl(irq, handler_خونه, control_خونه);
}

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                            cte_t *src_خونه, کلمه_t *بفر)
{
    إذا (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, بفر);
        کلمه_t شاخص = getSyscallArg(2, بفر);
        کلمه_t ژوروالی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خونه_ret_t lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع lu_ret.حالت;
        }

        cte_t *dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_خونه, src_خونه, trigger);
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
