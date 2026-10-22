/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <types.h>
#include <api/failures.h>

#include <arch/object/interrupt.h>

exception_t Arch_checkIRQ(word_t irq)
{
    གལ་སྲིད (irq > maxIRQ || irq == irqInvalid) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = maxIRQ;
        userError("Rejecting request for IRQ %u. IRQ is out of range [1..%u].", (ཧྲིལ)irq, maxIRQ);
        ལོག EXCEPTION_SYSCALL_ERROR;
    }
    ལོག EXCEPTION_NONE;
}

static exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, bool_t trigger)
{
#ifdef HAVE_SET_TRIGGER
    setIRQTrigger(irq, trigger);
#endif
    ལོག invokeIRQControl(irq, handlerSlot, controlSlot);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length,
                                            cte_t *srcSlot, word_t *buffer)
{
    གལ་སྲིད (invLabel == RISCVIRQIssueIRQHandlerTrigger) {
        གལ་སྲིད (length < 4 || current_extra_caps.excaprefs[0] == NULL) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ལོག EXCEPTION_SYSCALL_ERROR;
        }

        གལ་སྲིད (!config_set(HAVE_SET_TRIGGER)) {
            userError("This platform does not support setting the IRQ trigger");
            current_syscall_error.type = seL4_IllegalOperation;
            ལོག EXCEPTION_SYSCALL_ERROR;
        }

        word_t irq_w = getSyscallArg(0, buffer);
        irq_t irq = (irq_t) irq_w;
        bool_t trigger = !!getSyscallArg(1, buffer);
        word_t index = getSyscallArg(2, buffer);
        word_t depth = getSyscallArg(3, buffer);

        cap_t cnodeCap = current_extra_caps.excaprefs[0]->cap;

        exception_t status = Arch_checkIRQ(irq_w);
        གལ་སྲིད (status != EXCEPTION_NONE) {
            ལོག status;
        }

        གལ་སྲིད (isIRQActive(irq)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("Rejecting request for IRQ %u. Already active.", (ཧྲིལ)irq);
            ལོག EXCEPTION_SYSCALL_ERROR;
        }

        lookupSlot_ret_t lu_ret = lookupTargetSlot(cnodeCap, index, depth);
        གལ་སྲིད (lu_ret.status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap invalid: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ཧྲིལ)irq);
            ལོག lu_ret.status;
        }

        cte_t *destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        གལ་སྲིད (status != EXCEPTION_NONE) {
            userError("Target slot for new IRQ Handler cap not empty: cap %lu, IRQ %u.",
                      getExtraCPtr(buffer, 0), (ཧྲིལ)irq);
            ལོག status;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ལོག Arch_invokeIRQControl(irq, destSlot, srcSlot, trigger);
    } དེ་མིན {
        current_syscall_error.type = seL4_IllegalOperation;
        ལོག EXCEPTION_SYSCALL_ERROR;
    }
}
