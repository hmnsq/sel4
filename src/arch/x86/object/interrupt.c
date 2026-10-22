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
#جے_تعریف_شدہ CONFIG_IOMMU
            || i == irq_iommu
#جے_انت
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
exception_t Arch_checkIRQ(لفظ_t irq_w)
{
    إذا (config_سیٹ(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        أرجع EXCEPTION_NONE;
    }
    إذا (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } وإلا {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (لما)irq_isa_min, (لما)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    أرجع EXCEPTION_SYSCALL_ERROR;
}

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_تھاں, cte_t *control_تھاں, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    أرجع invokeIRQControl(irq, handler_تھاں, control_تھاں);
}

ساكن exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, لفظ_t ioapic, لفظ_t pin, لفظ_t level, لفظ_t polarity,
                                               لفظ_t سمتیہ,
                                               cte_t *handler_تھاں, cte_t *control_تھاں)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_منزل_سمتیہ(ioapic, pin, level, polarity, سمتیہ);
    أرجع Arch_invokeIRQControl(irq, handler_تھاں, control_تھاں, irqState);
}

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمائی, cte_t *src_تھاں, لفظ_t *بفر)
{
    لفظ_t اشاریہ, ڈونگھائی;
    cte_t *dest_تھاں;
    cap_t cnodeCap;
    lookup_تھاں_ret_t lu_ret;
    exception_t حالت;
    irq_t irq;
    لفظ_t سمتیہ;

    إذا (!config_سیٹ(CONFIG_IRQ_IOAPIC)) {
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

    إذا (لمائی < 7 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    ڈونگھائی = getSyscallArg(1, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, بفر);
    إذا (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (لما)irq, (لما)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    إذا (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (صحيح)irq);
        current_syscall_error.type = se_L4_Revoke_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سمتیہ = (لفظ_t)irq + IRQ_INT_وتھ;

    lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع lu_ret.حالت;
    }

    dest_تھاں = lu_ret.تھاں;

    حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
    إذا (حالت != EXCEPTION_NONE) {
        أرجع حالت;
    }

    بدل (invLabel) {
    حالة X86IRQIssueIRQHandlerIOAPIC: {
        لفظ_t ioapic = getSyscallArg(2, بفر);
        لفظ_t pin = getSyscallArg(3, بفر);
        لفظ_t level = getSyscallArg(4, بفر);
        لفظ_t polarity = getSyscallArg(5, بفر);

        حالت = ioapic_decode_map_pin_منزل_سمتیہ(ioapic, pin, level, polarity, سمتیہ);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, سمتیہ, dest_تھاں, src_تھاں);
    }
    اكسر;
    حالة X86IRQIssueIRQHandlerMSI: {
        لفظ_t pci_bus = getSyscallArg(2, بفر);
        لفظ_t pci_dev = getSyscallArg(3, بفر);
        لفظ_t pci_func = getSyscallArg(4, بفر);
        لفظ_t handle = getSyscallArg(5, بفر);
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

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_تھاں, src_تھاں, irqState);
    }
    اكسر;
    افتراضي:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
