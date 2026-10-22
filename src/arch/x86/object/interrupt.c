/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <kernel/boot.h>
#include <model/statedata.h>
#include <arch/object/interrupt.h>
#include <arch/api/invocation.h>
#include <linker.h>
#include <plat/machine/hardware.h>
#include <plat/machine/pci.h>

void Arch_irqStateInit(void)
{
    ຈຳນວນເຕັມ i = 0;
    ສຳລັບ (i = 0; i <= maxIRQ; i++) {
        ຖ້າ (i == irq_timer
#ifdef CONFIG_IOMMU
            || i == irq_iommu
#endif
           ) {
            x86KSIRQState[i] = x86_irq_state_irq_reserved_new();
        } ບໍ່ດັ່ງນັ້ນ {
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
    ຖ້າ (config_set(CONFIG_IRQ_PIC) && irq_w >= irq_isa_min && irq_w <= irq_isa_max) {
        ສົ່ງຄືນ EXCEPTION_NONE;
    }
    ຖ້າ (config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
    } ບໍ່ດັ່ງນັ້ນ {
        userError("IRQControl: IRQ %ld should be in range %ld - %ld", irq_w, (long)irq_isa_min, (long)irq_isa_max);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = irq_isa_min;
        current_syscall_error.rangeErrorMax = irq_isa_max;
    }
    ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
}

static exception_t Arch_invokeIRQControl(irq_t irq, cte_t *handlerSlot, cte_t *controlSlot, x86_irq_state_t irqState)
{
    updateIRQState(irq, irqState);
    ສົ່ງຄືນ invokeIRQControl(irq, handlerSlot, controlSlot);
}

static exception_t invokeIssueIRQHandlerIOAPIC(irq_t irq, word_t ioapic, word_t pin, word_t level, word_t polarity,
                                               word_t vector,
                                               cte_t *handlerSlot, cte_t *controlSlot)
{
    x86_irq_state_t irqState = x86_irq_state_irq_ioapic_new(ioapic, pin, level, polarity, 1);
    ioapic_map_pin_to_vector(ioapic, pin, level, polarity, vector);
    ສົ່ງຄືນ Arch_invokeIRQControl(irq, handlerSlot, controlSlot, irqState);
}

exception_t Arch_decodeIRQControlInvocation(word_t invLabel, word_t length, cte_t *srcSlot, word_t *buffer)
{
    word_t index, depth;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;
    irq_t irq;
    word_t vector;

    ຖ້າ (!config_set(CONFIG_IRQ_IOAPIC)) {
        userError("IRQControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
    }

    /* ensure we have a valid invocation before continuing any decoding */
    ຖ້າ (invLabel != X86IRQIssueIRQHandlerIOAPIC && invLabel != X86IRQIssueIRQHandlerMSI) {
        userError("IRQControl: Illegal operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
    }

    /* check the common parameters */

    ຖ້າ (length < 7 || current_extra_caps.excaprefs[0] == NULL) {
        userError("IRQControl: Truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    depth = getSyscallArg(1, buffer);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;
    irq = getSyscallArg(6, buffer);
    ຖ້າ (irq > irq_user_max - irq_user_min) {
        userError("IRQControl: Invalid irq %ld should be between 0-%ld", (long)irq, (long)(irq_user_max - irq_user_min));
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = irq_user_max - irq_user_min;
        ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
    }
    irq += irq_user_min;

    ຖ້າ (isIRQActive(irq)) {
        userError("IRQControl: IRQ %d is already active.", (ຈຳນວນເຕັມ)irq);
        current_syscall_error.type = seL4_RevokeFirst;
        ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
    }

    vector = (word_t)irq + IRQ_INT_OFFSET;

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ຖ້າ (lu_ret.status != EXCEPTION_NONE) {
        ສົ່ງຄືນ lu_ret.status;
    }

    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ຖ້າ (status != EXCEPTION_NONE) {
        ສົ່ງຄືນ status;
    }

    switch (invLabel) {
    pak_sa X86IRQIssueIRQHandlerIOAPIC: {
        word_t ioapic = getSyscallArg(2, buffer);
        word_t pin = getSyscallArg(3, buffer);
        word_t level = getSyscallArg(4, buffer);
        word_t polarity = getSyscallArg(5, buffer);

        status = ioapic_decode_map_pin_to_vector(ioapic, pin, level, polarity, vector);
        ຖ້າ (status != EXCEPTION_NONE) {
            ສົ່ງຄືນ status;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ສົ່ງຄືນ invokeIssueIRQHandlerIOAPIC(irq, ioapic, pin, level, polarity, vector, destSlot, srcSlot);
    }
    bha_nga;
    pak_sa X86IRQIssueIRQHandlerMSI: {
        word_t pci_bus = getSyscallArg(2, buffer);
        word_t pci_dev = getSyscallArg(3, buffer);
        word_t pci_func = getSyscallArg(4, buffer);
        word_t handle = getSyscallArg(5, buffer);
        x86_irq_state_t irqState;
        /* until we support msi interrupt remapping through vt-d we ignore the
         * vector and trust the user */

        ຖ້າ (pci_bus > PCI_BUS_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_BUS_MAX;
            ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
        }

        ຖ້າ (pci_dev > PCI_DEV_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_DEV_MAX;
            ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
        }

        ຖ້າ (pci_func > PCI_FUNC_MAX) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = 0;
            current_syscall_error.rangeErrorMax = PCI_FUNC_MAX;
            ສົ່ງຄືນ EXCEPTION_SYSCALL_ERROR;
        }

        irqState = x86_irq_state_irq_msi_new(pci_bus, pci_dev, pci_func, handle);

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ສົ່ງຄືນ Arch_invokeIRQControl(irq, destSlot, srcSlot, irqState);
    }
    bha_nga;
    default:
        /* the check at the start of this function should guarantee we do not get here */
        fail("IRQControl: Illegal operation");
    }
}
