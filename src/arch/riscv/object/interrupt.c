/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>

#شامل <arch/object/interrupt.h>

exception_t Arch_checkIRQ(لفظ_t irq)
{
    اگر (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (صحیح)irq, maxIRQ);
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس EXCEPTION_NONE;
}

ساکن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خانہ, cte_t *control_خانہ, bool_t trigger)
{
#اگر_تعریف_شدہ HAVE_سیٹ_TRIGGER
    سیٹ_IRQTrigger(irq, trigger);
#اگر_ختم
    واپس invokeIRQControl(irq, handler_خانہ, control_خانہ);
}

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                            cte_t *src_خانہ, لفظ_t *بفر)
{
    اگر (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
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
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, بفر);
        لفظ_t اشاریہ = getSyscallArg(2, بفر);
        لفظ_t گہرائی = getSyscallArg(3, بفر);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t حالت_2 = Arch_checkIRQ(irq_w);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        اگر (isIRQActive(irq)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("Rejecting request for IRQ %u. Already active.", (صحیح)irq);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        lookup_خانہ_ret_t lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)irq);
            واپس lu_ret.حالت_2;
        }

        cte_t *dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(بفر, 0), (صحیح)irq);
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس Arch_invokeIRQControl(irq, dest_خانہ, src_خانہ, trigger);
    } ورنہ {
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}
