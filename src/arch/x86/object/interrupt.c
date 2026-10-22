/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/boot.h>
#ضمن <model/statedata.h>
#ضمن <arch/object/interrupt.h>
#ضمن <arch/api/invocation.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <plat/machine/pci.h>

فراغ Arch_irqStateInit(فراغ)
{
    صحيح i = 0;
    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i == irq_timer
#که_تعریف_شوی CONFIG_IOMMU
            || i == irq_iommu
#د_که_پای
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } وإلا {
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
exception_t Arch_checkIRQ(کلمه_t irq_w)
{
    إذا (config_سېټ(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        أرجع EXCEPTION_NONE;
    }
    إذا (config_سېټ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } وإلا {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (اوږد)irq_isa_min, (اوږد)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    أرجع EXCEPTION_SYSCALL_ERROR;
}

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خونه, cte_t *control_خونه, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    أرجع invokeIRQControl(irq, handler_خونه, control_خونه);
}

ساكن exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, کلمه_t ioapic, کلمه_t pin, کلمه_t level, کلمه_t polarity,
                                               کلمه_t ویکټور,
                                               cte_t *handler_خونه, cte_t *control_خونه)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_منزل_ویکټور(ioapic, pin, level, polarity, ویکټور);
    أرجع Arch_invokeIRQControl(irq, handler_خونه, control_خونه, irqState);
}

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cte_t *src_خونه, کلمه_t *بفر)
{
    کلمه_t شاخص, ژوروالی;
    cte_t *dest_خونه;
    cap_t cnodeCap;
    lookup_خونه_ret_t lu_ret;
    exception_t حالت;
    irq_t irq;
    کلمه_t ویکټور;

    إذا (!config_سېټ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    إذا (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    إذا (اوږدتیا < 7 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, بفر);
    ژوروالی = getSyscallArg(1, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, بفر);
    إذا (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (اوږد)irq, (اوږد)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    إذا (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (صحيح)irq);
        current_syscall_error.type = se_L4_Revoke_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ویکټور = (کلمه_t)irq + IRQ_INT_دننه_واټن;

    lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع lu_ret.حالت;
    }

    dest_خونه = lu_ret.خونه;

    حالت = ensure_تشه_خونه(dest_خونه);
    إذا (حالت != EXCEPTION_NONE) {
        أرجع حالت;
    }

    بدل (invLabel) {
    حالة X86IRQIssueIRQHandlerIOAPIC: {
        کلمه_t ioapic = getSyscallArg(2, بفر);
        کلمه_t pin = getSyscallArg(3, بفر);
        کلمه_t level = getSyscallArg(4, بفر);
        کلمه_t polarity = getSyscallArg(5, بفر);

        حالت = ioapic_decode_map_pin_منزل_ویکټور(ioapic, pin, level, polarity, ویکټور);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, ویکټور, dest_خونه, src_خونه);
    }
    اكسر;
    حالة X86IRQIssueIRQHandlerMSI: {
        کلمه_t pci_bus = getSyscallArg(2, بفر);
        کلمه_t pci_dev = getSyscallArg(3, بفر);
        کلمه_t pci_func = getSyscallArg(4, بفر);
        کلمه_t handle = getSyscallArg(5, بفر);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        إذا (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_خونه, src_خونه, irqState);
    }
    اكسر;
    افتراضي:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
