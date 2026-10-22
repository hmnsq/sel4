/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <kernel/boot.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <arch/object/interrupt.h>
#অন্তর্ভাব <arch/api/invocation.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <plat/machine/pci.h>

শূন্য Arch_irqStateInit(শূন্য)
{
    পূর্ণসংখ্যা i = 0;
    জন্য (i = 0; i <= maxIRQ; i++) {
        যদি (i == irq_timer
#ifdef CONFIG_IOMMU
            || i == irq_iommu
#endif
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } অন্যথা {
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
    যদি (config_set(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        ফেরত EXCEPTION_NONE;
    }
    যদি (config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } অন্যথা {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (long)irq_isa_min, (long)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    ফেরত EXCEPTION_SYSCALL_ERROR;
}

স্থির exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    ফেরত invokeIRQControl(irq, handlerSlot, controlSlot);
}

স্থির exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t vector,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_to_vector(ioapic, pin, level, polarity, vector);
    ফেরত Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t দৈর্ঘ্য, cte_t *srcSlot, word_t *বাফার)
{
    word_t সূচক, depth;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;
    irq_t irq;
    word_t vector;

    যদি (!config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    যদি (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    যদি (দৈর্ঘ্য < 7 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    সূচক = getSyscallArg(0, বাফার);
    depth = getSyscallArg(1, বাফার);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, বাফার);
    যদি (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (long)irq, (long)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    যদি (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (পূর্ণসংখ্যা)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vector = (word_t)irq + IRQ_INT_OFFSET;

    lu_ret = lookupTargetSlot(cnodeCap, সূচক, depth);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        ফেরত lu_ret.status;
    }

    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    যদি (status != EXCEPTION_NONE) {
        ফেরত status;
    }

    নির্বাচন (invLabel) {
    ক্ষেত্র X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, বাফার);
        word_t pin = getSyscallArg(3, বাফার);
        word_t level = getSyscallArg(4, বাফার);
        word_t polarity = getSyscallArg(5, বাফার);

        status = ioapic_decode_map_pin_to_vector(ioapic, pin, level, polarity, vector);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, vector, destSlot, srcSlot);
    }
    বিরতি;
    ক্ষেত্র X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, বাফার);
        word_t pci_dev = getSyscallArg(3, বাফার);
        word_t pci_func = getSyscallArg(4, বাফার);
        word_t handle = getSyscallArg(5, বাফার);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        যদি (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    বিরতি;
    পূর্বনির্ধারিত:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
