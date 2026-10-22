/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <kernel/boot.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/object/interrupt.h>
#அந்தர்பாவ <arch/api/invocation.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <plat/machine/pci.h>

வெறுமை Arch_irqStateInit(வெறுமை)
{
    முழு i = 0;
    க்கு (i = 0; i <= maxIRQ; i++) {
        எனில் (i == irq_timer
#ifdef CONFIG_IOMMU
            || i == irq_iommu
#endif
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } இல்லையெனில் {
            x86KSIRQState[i] = x86_irq_state_irq_free_new();
        }
    }
}

/* for x86, the IRQIssueIRQHandler is only allowed to
 * issue a handler for IRQ 0-15, the isa IRQs.
 * Use getIRQHandlerIOAPIC and getIRQHandlerMSI for
 * the IRQs >= 16. Additionally these IRQs only exist
 * if using the legacy PIC interrupt
 */
exception_t Arch_checkIRQ(word_t irq_w)
{
    எனில் (config_set(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        திரும்பு EXCEPTION_NONE;
    }
    எனில் (config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } இல்லையெனில் {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (long)irq_isa_min, (long)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    திரும்பு EXCEPTION_SYSCALL_ERROR;
}

நிலையான exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    திரும்பு invokeIRQControl(irq, handlerSlot, controlSlot);
}

நிலையான exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t vector,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_to_vector(ioapic, pin, level, polarity, vector);
    திரும்பு Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length, cte_t *srcSlot, word_t *buffer)
{
    word_t index, depth;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;
    irq_t irq;
    word_t vector;

    எனில் (!config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    எனில் (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    எனில் (length < 7 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    depth = getSyscallArg(1, buffer);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, buffer);
    எனில் (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (long)irq, (long)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    எனில் (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (முழு)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    vector = (word_t)irq + IRQ_INT_OFFSET;

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        திரும்பு lu_ret.status;
    }

    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    எனில் (status != EXCEPTION_NONE) {
        திரும்பு status;
    }

    தேர்வு (invLabel) {
    நிலை X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, buffer);
        word_t pin = getSyscallArg(3, buffer);
        word_t level = getSyscallArg(4, buffer);
        word_t polarity = getSyscallArg(5, buffer);

        status = ioapic_decode_map_pin_to_vector(ioapic, pin, level, polarity, vector);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, vector, destSlot, srcSlot);
    }
    நிறுத்து;
    நிலை X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, buffer);
        word_t pci_dev = getSyscallArg(3, buffer);
        word_t pci_func = getSyscallArg(4, buffer);
        word_t handle = getSyscallArg(5, buffer);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        எனில் (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    நிறுத்து;
    இயல்பு:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
