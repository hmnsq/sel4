/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <util.h>
#include <stdint.h>
#include <plat/machine.h>
BOOT_CODE void initIRQController(void)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    intc_regs->bfEnableBasicIRQs = BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_CODE void cpu_initLocalIRQController(void) {}

static inline irq_t getActiveIRQ(void)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    nếu (pending == 0) {
        trả_về irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    nếu (irq != INTERRUPT_CORE_GPU) {
        trả_về irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    nếu (pending) {
        trả_về (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    nếu (pending) {
        trả_về (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    nếu (pending) {
        trả_về (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    trả_về irqInvalid;
}

static inline void maskInterrupt(bool_t disable, irq_t irq)
{
    switch (irq) {
    case INTERRUPT_CORE_CNTPSIRQ :
    case INTERRUPT_CORE_CNTPNSIRQ:
    case INTERRUPT_CORE_CNTHPIRQ :
    case INTERRUPT_CORE_CNTVIRQ  :
        nếu (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } không_thì {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        trả_về;
    case INTERRUPT_CORE_MAILBOX_0:
    case INTERRUPT_CORE_MAILBOX_1:
    case INTERRUPT_CORE_MAILBOX_2:
    case INTERRUPT_CORE_MAILBOX_3:
        nếu (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } không_thì {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        trả_về;
    case INTERRUPT_CORE_LOCAL_TIMER:
        nếu (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } không_thì {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        trả_về;
    case INTERRUPT_CORE_GPU:
    // Not maskable
    case INTERRUPT_CORE_PMU:
    // Not currently handled
    case INTERRUPT_CORE_AXI:
        // Not currently handled
        trả_về;
    default:
        break;
    }
    nếu (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        trả_về;
    }

    nếu (irq < NORMAL_IRQ_OFFSET) {
        nếu (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } không_thì {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } không_thì nếu (irq <= maxIRQ) {
        số_nguyên normal_irq = irq - NORMAL_IRQ_OFFSET;
        số_nguyên index = normal_irq / 32;
        nếu (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } không_thì {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
