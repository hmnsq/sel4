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
#اگر_تعریف_شده CONFIG_IOMMU
            || i == irq_iommu
#پایان_اگر
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
    إذا (config_مجموعه(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        أرجع EXCEPTION_NONE;
    }
    إذا (config_مجموعه(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } وإلا {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (دراز)irq_isa_min, (دراز)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    أرجع EXCEPTION_SYSCALL_ERROR;
}

ساكن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_جایگاه, cte_t *control_جایگاه, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    أرجع invokeIRQControl(irq, handler_جایگاه, control_جایگاه);
}

ساكن exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, کلمه_t ioapic, کلمه_t pin, کلمه_t level, کلمه_t polarity,
                                               کلمه_t وکتور,
                                               cte_t *handler_جایگاه, cte_t *control_جایگاه)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_مقصد_وکتور(ioapic, pin, level, polarity, وکتور);
    أرجع Arch_invokeIRQControl(irq, handler_جایگاه, control_جایگاه, irqState);
}

exception_t Arch_decodeIRQControlInvocation(کلمه_t invLabel, کلمه_t درازی, cte_t *src_جایگاه, کلمه_t *میانگیر)
{
    کلمه_t شاخص, عمق;
    cte_t *dest_جایگاه;
    cap_t cnodeCap;
    lookup_جایگاه_ret_t lu_ret;
    exception_t وضعیت;
    irq_t irq;
    کلمه_t وکتور;

    إذا (!config_مجموعه(CONFIG_IRQ_IOAPIC)) {
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

    إذا (درازی < 7 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, میانگیر);
    عمق = getSyscallArg(1, میانگیر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, میانگیر);
    إذا (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (دراز)irq, (دراز)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    إذا (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (صحيح)irq);
        current_syscall_error.type = se_L4_Revoke_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    وکتور = (کلمه_t)irq + IRQ_INT_جابه_جایی;

    lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        أرجع lu_ret.وضعیت;
    }

    dest_جایگاه = lu_ret.جایگاه;

    وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
    إذا (وضعیت != EXCEPTION_NONE) {
        أرجع وضعیت;
    }

    بدل (invLabel) {
    حالة X86IRQIssueIRQHandlerIOAPIC: {
        کلمه_t ioapic = getSyscallArg(2, میانگیر);
        کلمه_t pin = getSyscallArg(3, میانگیر);
        کلمه_t level = getSyscallArg(4, میانگیر);
        کلمه_t polarity = getSyscallArg(5, میانگیر);

        وضعیت = ioapic_decode_map_pin_مقصد_وکتور(ioapic, pin, level, polarity, وکتور);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, وکتور, dest_جایگاه, src_جایگاه);
    }
    اكسر;
    حالة X86IRQIssueIRQHandlerMSI: {
        کلمه_t pci_bus = getSyscallArg(2, میانگیر);
        کلمه_t pci_dev = getSyscallArg(3, میانگیر);
        کلمه_t pci_func = getSyscallArg(4, میانگیر);
        کلمه_t handle = getSyscallArg(5, میانگیر);
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

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع Arch_invokeIRQControl(irq, dest_جایگاه, src_جایگاه, irqState);
    }
    اكسر;
    افتراضي:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
