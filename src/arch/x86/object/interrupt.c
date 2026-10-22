/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/object/interrupt.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <plat/machine/pci.h>

शून्य Arch_irqStateInit(शून्य)
{
    पूर्णाङ्क i = 0;
    प्रति (i = 0; i <= maxIRQ; i++) {
        यदि (i == irq_timer
#यदि_परिभाषितम् CONFIG_IOMMU
            || i == irq_iommu
#यदि_अन्तः
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
    यदि (config_समुच्चयः(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        प्रत्यागम EXCEPTION_NONE;
    }
    यदि (config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } अन्यथा {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (दीर्घ)irq_isa_min, (दीर्घ)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    प्रत्यागम EXCEPTION_SYSCALL_ERROR;
}

स्थिर exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    प्रत्यागम invokeIRQControl(irq, handlerSlot, controlSlot);
}

स्थिर exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t सदिशम्,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_गन्तव्यम्_सदिशम्(ioapic, pin, level, polarity, सदिशम्);
    प्रत्यागम Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t दीर्घता, cte_t *srcSlot, word_t *बफर)
{
    word_t सूचकांक, गभीरता;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t स्थितिः;
    irq_t irq;
    word_t सदिशम्;

    यदि (!config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    यदि (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    यदि (दीर्घता < 7 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    गभीरता = getSyscallArg(1, बफर);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, बफर);
    यदि (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (दीर्घ)irq, (दीर्घ)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    यदि (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (पूर्णाङ्क)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    सदिशम् = (word_t)irq + IRQ_INT_विस्थापनम्;

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम lu_ret.स्थितिः;
    }

    destSlot = lu_ret.slot;

    स्थितिः = ensure_रिक्तानि_Slot(destSlot);
    यदि (स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम स्थितिः;
    }

    विकल्प (invLabel) {
    पक्ष X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, बफर);
        word_t pin = getSyscallArg(3, बफर);
        word_t level = getSyscallArg(4, बफर);
        word_t polarity = getSyscallArg(5, बफर);

        स्थितिः = ioapic_decode_map_pin_गन्तव्यम्_सदिशम्(ioapic, pin, level, polarity, सदिशम्);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, सदिशम्, destSlot, srcSlot);
    }
    भङ्ग;
    पक्ष X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, बफर);
        word_t pci_dev = getSyscallArg(3, बफर);
        word_t pci_func = getSyscallArg(4, बफर);
        word_t handle = getSyscallArg(5, बफर);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        यदि (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    भङ्ग;
    स्वभाव:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
