/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <kernel/boot.h>
#समावेश <model/statedata.h>
#समावेश <arch/object/interrupt.h>
#समावेश <arch/api/invocation.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>
#समावेश <plat/machine/pci.h>

व्योम Arch_irqStateInit(व्योम)
{
    पूर्णांक i = 0;
    क्रम (i = 0; i <= maxIRQ; i++) {
        अगर (i == irq_timer
#अगर_घोषित CONFIG_IOMMU
            || i == irq_iommu
#पूर्ण_अगर
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } अन्यथा {
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
    अगर (config_set(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        वापस EXCEPTION_NONE;
    }
    अगर (config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } अन्यथा {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (दीर्घ)irq_isa_min, (दीर्घ)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    वापस EXCEPTION_SYSCALL_ERROR;
}

अटल exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    वापस invokeIRQControl(irq, handlerSlot, controlSlot);
}

अटल exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t vector,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_to_vector(ioapic, pin, level, polarity, vector);
    वापस Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t लंबाई, cte_t *srcSlot, word_t *बफर)
{
    word_t सूचकांक, depth;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;
    irq_t irq;
    word_t vector;

    अगर (!config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    अगर (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    अगर (लंबाई < 7 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    depth = getSyscallArg(1, बफर);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, बफर);
    अगर (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (दीर्घ)irq, (दीर्घ)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    अगर (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (पूर्णांक)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vector = (word_t)irq + IRQ_INT_OFFSET;

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
    अगर (lu_ret.status != EXCEPTION_NONE) {
        वापस lu_ret.status;
    }

    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    अगर (status != EXCEPTION_NONE) {
        वापस status;
    }

    चयन (invLabel) {
    हाल X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, बफर);
        word_t pin = getSyscallArg(3, बफर);
        word_t level = getSyscallArg(4, बफर);
        word_t polarity = getSyscallArg(5, बफर);

        status = ioapic_decode_map_pin_to_vector(ioapic, pin, level, polarity, vector);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, vector, destSlot, srcSlot);
    }
    अवरोध;
    हाल X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, बफर);
        word_t pci_dev = getSyscallArg(3, बफर);
        word_t pci_func = getSyscallArg(4, बफर);
        word_t handle = getSyscallArg(5, बफर);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        अगर (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    अवरोध;
    शेष:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
