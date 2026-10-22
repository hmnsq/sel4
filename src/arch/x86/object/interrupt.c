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
    ಪೂರ್ಣಾಂಕ i = 0;
    ಗಾಗಿ (i = 0; i <= maxIRQ; i++) {
        ಆದರೆ (i == irq_timer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
            || i == irq_iommu
#ಆದರೆ_ಅಂತ್ಯ
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } ಇಲ್ಲದಿದ್ದರೆ {
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
exception_t Arch_checkIRQ(ಶಬ್ದ_t irq_w)
{
    ಆದರೆ (config_ಗಣ(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }
    ಆದರೆ (config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } ಇಲ್ಲದಿದ್ದರೆ {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (ದೀರ್ಘ)irq_isa_min, (ದೀರ್ಘ)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
}

ಸ್ಥಿರ exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    ಹಿಂತಿರುಗಿ invokeIRQControl(irq, handler_ಸ್ಥಾನ, control_ಸ್ಥಾನ);
}

ಸ್ಥಿರ exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, ಶಬ್ದ_t ioapic, ಶಬ್ದ_t pin, ಶಬ್ದ_t level, ಶಬ್ದ_t polarity,
                                               ಶಬ್ದ_t ಸದಿಶ,
                                               cte_t *handler_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_ಗಮ್ಯ_ಸದಿಶ(ioapic, pin, level, polarity, ಸದಿಶ);
    ಹಿಂತಿರುಗಿ Arch_invokeIRQControl(irq, handler_ಸ್ಥಾನ, control_ಸ್ಥಾನ, irqState);
}

exception_t Arch_decodeIRQControlInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *src_ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ;
    cte_t *dest_ಸ್ಥಾನ;
    cap_t cnodeCap;
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    exception_t ಸ್ಥಿತಿ_2;
    irq_t irq;
    ಶಬ್ದ_t ಸದಿಶ;

    ಆದರೆ (!config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    ಆದರೆ (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    ಆದರೆ (ಉದ್ದಳತೆ < 7 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
    ಆಳ = getSyscallArg(1, ಬಫರ್);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, ಬಫರ್);
    ಆದರೆ (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (ದೀರ್ಘ)irq, (ದೀರ್ಘ)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    ಆದರೆ (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (ಪೂರ್ಣಾಂಕ)irq);
        current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಸದಿಶ = (ಶಬ್ದ_t)irq + IRQ_INT_ಸ್ಥಾನಾಂತರ;

    lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
    }

    dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

    ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ X86IRQIssueIRQHandlerIOAPIC: {
        ಶಬ್ದ_t ioapic = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t pin = getSyscallArg(3, ಬಫರ್);
        ಶಬ್ದ_t level = getSyscallArg(4, ಬಫರ್);
        ಶಬ್ದ_t polarity = getSyscallArg(5, ಬಫರ್);

        ಸ್ಥಿತಿ_2 = ioapic_decode_map_pin_ಗಮ್ಯ_ಸದಿಶ(ioapic, pin, level, polarity, ಸದಿಶ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, ಸದಿಶ, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ);
    }
    ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86IRQIssueIRQHandlerMSI: {
        ಶಬ್ದ_t pci_bus = getSyscallArg(2, ಬಫರ್);
        ಶಬ್ದ_t pci_dev = getSyscallArg(3, ಬಫರ್);
        ಶಬ್ದ_t pci_func = getSyscallArg(4, ಬಫರ್);
        ಶಬ್ದ_t handle = getSyscallArg(5, ಬಫರ್);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        ಆದರೆ (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ Arch_invokeIRQControl(irq, dest_ಸ್ಥಾನ, src_ಸ್ಥಾನ, irqState);
    }
    ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
