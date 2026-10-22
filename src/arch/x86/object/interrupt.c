/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pci.h>

ਖਾਲੀ Arch_irqStateInit(ਖਾਲੀ)
{
    ਪੂਰਨ i = 0;
    ਲਈ (i = 0; i <= maxIRQ; i++) {
        ਜੇ (i == irq_timer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
            || i == irq_iommu
#ਜੇ_ਅੰਤ
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } ਨਹੀਂਤਾਂ {
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
exception_t Arch_checkIRQ(ਸ਼ਬਦ_t irq_w)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        ਵਾਪਸ EXCEPTION_NONE;
    }
    ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } ਨਹੀਂਤਾਂ {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (ਲੰਮਾ)irq_isa_min, (ਲੰਮਾ)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
}

ਸਥਿਰ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ਥਾਂ, cte_t *control_ਥਾਂ, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    ਵਾਪਸ invokeIRQControl(irq, handler_ਥਾਂ, control_ਥਾਂ);
}

ਸਥਿਰ exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, ਸ਼ਬਦ_t ioapic, ਸ਼ਬਦ_t pin, ਸ਼ਬਦ_t level, ਸ਼ਬਦ_t polarity,
                                               ਸ਼ਬਦ_t ਸਦਿਸ਼,
                                               cte_t *handler_ਥਾਂ, cte_t *control_ਥਾਂ)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_ਮੰਜ਼ਿਲ_ਸਦਿਸ਼(ioapic, pin, level, polarity, ਸਦਿਸ਼);
    ਵਾਪਸ Arch_invokeIRQControl(irq, handler_ਥਾਂ, control_ਥਾਂ, irqState);
}

exception_t Arch_decodeIRQControlInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *src_ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ;
    cte_t *dest_ਥਾਂ;
    cap_t cnodeCap;
    lookup_ਥਾਂ_ret_t lu_ret;
    exception_t ਸਥਿਤੀ;
    irq_t irq;
    ਸ਼ਬਦ_t ਸਦਿਸ਼;

    ਜੇ (!config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    ਜੇ (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    ਜੇ (ਲੰਮਾਈ < 7 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
    ਡੂੰਘਾਈ = getSyscallArg(1, ਬਫਰ);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, ਬਫਰ);
    ਜੇ (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (ਲੰਮਾ)irq, (ਲੰਮਾ)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    ਜੇ (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (ਪੂਰਨ)irq);
        current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸਦਿਸ਼ = (ਸ਼ਬਦ_t)irq + IRQ_INT_ਵਿੱਥ;

    lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
    }

    dest_ਥਾਂ = lu_ret.ਥਾਂ;

    ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਚੋਣ (invLabel) {
    ਹਾਲਤ X86IRQIssueIRQHandlerIOAPIC: {
        ਸ਼ਬਦ_t ioapic = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t pin = getSyscallArg(3, ਬਫਰ);
        ਸ਼ਬਦ_t level = getSyscallArg(4, ਬਫਰ);
        ਸ਼ਬਦ_t polarity = getSyscallArg(5, ਬਫਰ);

        ਸਥਿਤੀ = ioapic_decode_map_pin_ਮੰਜ਼ਿਲ_ਸਦਿਸ਼(ioapic, pin, level, polarity, ਸਦਿਸ਼);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, ਸਦਿਸ਼, dest_ਥਾਂ, src_ਥਾਂ);
    }
    ਰੋਕੋ;
    ਹਾਲਤ X86IRQIssueIRQHandlerMSI: {
        ਸ਼ਬਦ_t pci_bus = getSyscallArg(2, ਬਫਰ);
        ਸ਼ਬਦ_t pci_dev = getSyscallArg(3, ਬਫਰ);
        ਸ਼ਬਦ_t pci_func = getSyscallArg(4, ਬਫਰ);
        ਸ਼ਬਦ_t handle = getSyscallArg(5, ਬਫਰ);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        ਜੇ (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ Arch_invokeIRQControl(irq, dest_ਥਾਂ, src_ਥਾਂ, irqState);
    }
    ਰੋਕੋ;
    ਮੂਲ:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
