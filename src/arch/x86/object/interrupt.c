/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pci.h>

ಶೂನ್ಯ Arch_irqStateInit(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ i = 0;
    ಪ್ರತಿ (i = 0; i <= maxIRQ; i++) {
        ಯದಿ (i == irq_timer
#ifdef CONFIG_IOMMU
            || i == irq_iommu
#endif
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } ಅನ್ಯಥಾ {
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
    ಯದಿ (config_set(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }
    ಯದಿ (config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } ಅನ್ಯಥಾ {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (long)irq_isa_min, (long)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
}

ಸ್ಥಿರ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    ಪ್ರತ್ಯಾಗಮ invokeIRQControl(irq, handlerSlot, controlSlot);
}

ಸ್ಥಿರ exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t vector,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_to_vector(ioapic, pin, level, polarity, vector);
    ಪ್ರತ್ಯಾಗಮ Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
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

    ಯದಿ (!config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    ಯದಿ (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    ಯದಿ (length < 7 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    depth = getSyscallArg(1, buffer);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, buffer);
    ಯದಿ (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (long)irq, (long)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    ಯದಿ (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (ಪೂರ್ಣಾಙ್ಕ)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    vector = (word_t)irq + IRQ_INT_OFFSET;

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ lu_ret.status;
    }

    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ಯದಿ (status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ status;
    }

    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, buffer);
        word_t pin = getSyscallArg(3, buffer);
        word_t level = getSyscallArg(4, buffer);
        word_t polarity = getSyscallArg(5, buffer);

        status = ioapic_decode_map_pin_to_vector(ioapic, pin, level, polarity, vector);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, vector, destSlot, srcSlot);
    }
    ಭಙ್ಗ;
    ಪಕ್ಷ X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, buffer);
        word_t pci_dev = getSyscallArg(3, buffer);
        word_t pci_func = getSyscallArg(4, buffer);
        word_t handle = getSyscallArg(5, buffer);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        ಯದಿ (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
