/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>

#ضمن <arch/object/interrupt.h>

exception_t Arch_checkIRQ(لفظ_t irq)
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

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_تھاں, cte_t *control_تھاں, bool_t trigger)
{
#جے_تعریف_شدہ HAVE_سیٹ_TRIGGER
    سیٹ_IRQTrigger(irq, trigger);
#جے_انت
    أرجع invokeIRQControl(irq, handler_تھاں, control_تھاں);
}

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                            cte_t *src_تھاں, لفظ_t *بفر)
{
    إذا (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t ڈونگھائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_تھاں_ret_t lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع lu_ret.حالت;
        }

        cte_t *dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_تھاں, src_تھاں, trigger);
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
