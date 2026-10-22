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

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_جایگاه, cte_t *control_جایگاه, bool_t trigger)
{
#اگر_تعریف_شده HAVE_مجموعه_TRIGGER
    مجموعه_IRQTrigger(irq, trigger);
#پایان_اگر
    أرجع invokeIRQControl(irq, handler_جایگاه, control_جایگاه);
}

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t درازی,
                                            cte_t *src_جایگاه, کلمه_t *میانگیر)
{
    إذا (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, میانگیر);
        کلمه_t شاخص = getSyscallArg(2, میانگیر);
        کلمه_t عمق = getSyscallArg(3, میانگیر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t وضعیت = Arch_checkIRQ(irq_w);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        إذا (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("Rejecting request for IRQ %u. Already active.", (صحيح)irq);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        lookup_جایگاه_ret_t lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)irq);
            أرجع lu_ret.وضعیت;
        }

        cte_t *dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(میانگیر, 0), (صحيح)irq);
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_جایگاه, src_جایگاه, trigger);
    } وإلا {
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
