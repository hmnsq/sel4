/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/boot.h>
#شامل <model/statedata.h>
#شامل <arch/object/interrupt.h>
#شامل <arch/api/invocation.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>
#شامل <plat/machine/pci.h>

خالی Arch_irqStateInit(خالی)
{
    صحیح i = 0;
    برائے (i = 0; i <= maxIRQ; i++) {
        اگر (i == irq_timer
#اگر_تعریف_شدہ CONFIG_IOMMU
            || i == irq_iommu
#اگر_ختم
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } ورنہ {
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
    اگر (config_سیٹ(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        واپس EXCEPTION_NONE;
    }
    اگر (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } ورنہ {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (طویل)irq_isa_min, (طویل)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    واپس EXCEPTION_SYSCALL_ERROR;
}

ساکن exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handler_خانہ, cte_t *control_خانہ, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    واپس invokeIRQControl(irq, handler_خانہ, control_خانہ);
}

ساکن exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, لفظ_t ioapic, لفظ_t pin, لفظ_t level, لفظ_t polarity,
                                               لفظ_t سمتیہ,
                                               cte_t *handler_خانہ, cte_t *control_خانہ)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_مقصد_سمتیہ(ioapic, pin, level, polarity, سمتیہ);
    واپس Arch_invokeIRQControl(irq, handler_خانہ, control_خانہ, irqState);
}

exception_t Arch_decodeIRQControlInvocation(لفظ_t invLabel, لفظ_t لمبائی, cte_t *src_خانہ, لفظ_t *بفر)
{
    لفظ_t اشاریہ, گہرائی;
    cte_t *dest_خانہ;
    cap_t cnodeCap;
    lookup_خانہ_ret_t lu_ret;
    exception_t حالت_2;
    irq_t irq;
    لفظ_t سمتیہ;

    اگر (!config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    اگر (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    اگر (لمبائی < 7 || current_extra_caps.excaprefs[0] == تہی) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    گہرائی = getSyscallArg(1, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, بفر);
    اگر (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (طویل)irq, (طویل)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    اگر (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (صحیح)irq);
        current_syscall_error.type = se_L4_Revoke_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سمتیہ = (لفظ_t)irq + IRQ_INT_انحراف;

    lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        واپس lu_ret.حالت_2;
    }

    dest_خانہ = lu_ret.خانہ;

    حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
    اگر (حالت_2 != EXCEPTION_NONE) {
        واپس حالت_2;
    }

    انتخاب (invLabel) {
    حالت X86IRQIssueIRQHandlerIOAPIC: {
        لفظ_t ioapic = getSyscallArg(2, بفر);
        لفظ_t pin = getSyscallArg(3, بفر);
        لفظ_t level = getSyscallArg(4, بفر);
        لفظ_t polarity = getSyscallArg(5, بفر);

        حالت_2 = ioapic_decode_map_pin_مقصد_سمتیہ(ioapic, pin, level, polarity, سمتیہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, سمتیہ, dest_خانہ, src_خانہ);
    }
    توڑو;
    حالت X86IRQIssueIRQHandlerMSI: {
        لفظ_t pci_bus = getSyscallArg(2, بفر);
        لفظ_t pci_dev = getSyscallArg(3, بفر);
        لفظ_t pci_func = getSyscallArg(4, بفر);
        لفظ_t handle = getSyscallArg(5, بفر);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        اگر (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس Arch_invokeIRQControl(irq, dest_خانہ, src_خانہ, irqState);
    }
    توڑو;
    طےشدہ:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
