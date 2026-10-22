/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/object/interrupt.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <plat/machine/pci.h>

శూన్య Arch_irqStateInit(శూన్య)
{
    పూర్ణాంకం i = 0;
    కొరకు (i = 0; i <= maxIRQ; i++) {
        ఒకవేళ (i == irq_timer
#నిర్వచితమైతే CONFIG_IOMMU
            || i == irq_iommu
#అయితే_ముగింపు
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } లేకపోతే {
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
exception_t Arch_checkIRQ(మాట_t irq_w)
{
    ఒకవేళ (config_సమితి(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        తిరిగి EXCEPTION_NONE;
    }
    ఒకవేళ (config_సమితి(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } లేకపోతే {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (దీర్ఘ)irq_isa_min, (దీర్ఘ)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    తిరిగి EXCEPTION_SYSCALL_ERROR;
}

స్థిర exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_స్థానం, cte_t *control_స్థానం, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    తిరిగి invokeIRQControl(irq, handler_స్థానం, control_స్థానం);
}

స్థిర exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, మాట_t ioapic, మాట_t pin, మాట_t level, మాట_t polarity,
                                               మాట_t సదిశ,
                                               cte_t *handler_స్థానం, cte_t *control_స్థానం)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_గమ్యం_సదిశ(ioapic, pin, level, polarity, సదిశ);
    తిరిగి Arch_invokeIRQControl(irq, handler_స్థానం, control_స్థానం, irqState);
}

exception_t Arch_decodeIRQControlInvocation(మాట_t invLabel, మాట_t నిడివి, cte_t *src_స్థానం, మాట_t *బఫరు)
{
    మాట_t సూచీ, లోతు;
    cte_t *dest_స్థానం;
    cap_t cnodeCap;
    lookup_స్థానం_ret_t lu_ret;
    exception_t స్థితి_2;
    irq_t irq;
    మాట_t సదిశ;

    ఒకవేళ (!config_సమితి(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    ఒకవేళ (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    ఒకవేళ (నిడివి < 7 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సూచీ = getSyscallArg(0, బఫరు);
    లోతు = getSyscallArg(1, బఫరు);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, బఫరు);
    ఒకవేళ (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (దీర్ఘ)irq, (దీర్ఘ)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    ఒకవేళ (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (పూర్ణాంకం)irq);
        current_syscall_error.type = se_L4_Revoke_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సదిశ = (మాట_t)irq + IRQ_INT_స్థానభ్రంశం;

    lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి lu_ret.స్థితి_2;
    }

    dest_స్థానం = lu_ret.స్థానం;

    స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి స్థితి_2;
    }

    ఎంపిక (invLabel) {
    స్థితి X86IRQIssueIRQHandlerIOAPIC: {
        మాట_t ioapic = getSyscallArg(2, బఫరు);
        మాట_t pin = getSyscallArg(3, బఫరు);
        మాట_t level = getSyscallArg(4, బఫరు);
        మాట_t polarity = getSyscallArg(5, బఫరు);

        స్థితి_2 = ioapic_decode_map_pin_గమ్యం_సదిశ(ioapic, pin, level, polarity, సదిశ);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, సదిశ, dest_స్థానం, src_స్థానం);
    }
    ఆపు;
    స్థితి X86IRQIssueIRQHandlerMSI: {
        మాట_t pci_bus = getSyscallArg(2, బఫరు);
        మాట_t pci_dev = getSyscallArg(3, బఫరు);
        మాట_t pci_func = getSyscallArg(4, బఫరు);
        మాట_t handle = getSyscallArg(5, బఫరు);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        ఒకవేళ (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి Arch_invokeIRQControl(irq, dest_స్థానం, src_స్థానం, irqState);
    }
    ఆపు;
    అప్రమేయ:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
