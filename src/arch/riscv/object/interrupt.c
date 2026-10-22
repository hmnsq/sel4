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

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_جڳهه, cte_t *control_جڳهه, bool_t trigger)
{
#جيڪڏهن_وصف_ٿيل HAVE_سيٽ_TRIGGER
    سيٽ_IRQTrigger(irq, trigger);
#جيڪڏهن_پورو
    أرجع invokeIRQControl(irq, handler_جڳهه, control_جڳهه);
}

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                            cte_t *src_جڳهه, لفظ_t *بفر)
{
    إذا (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاريو = getSyscallArg(2, بفر);
        لفظ_t اونهائي = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت = Arch_checkIRQ(irq_w);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جڳهه_ret_t lu_ret = lookup_هدف_جڳهه(cnodeCap, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع lu_ret.حالت;
        }

        cte_t *dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحيح)irq);
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_جڳهه, src_جڳهه, trigger);
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
