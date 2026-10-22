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
    если (pending == 0) {
        вернуть irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    если (irq != INTERRUPT_CORE_GPU) {
        вернуть irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    если (pending) {
        вернуть (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    если (pending) {
        вернуть (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    если (pending) {
        вернуть (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    вернуть irqInvalid;
}

static inline void maskInterrupt(bool_t disable, irq_t irq)
{
    switch (irq) {
    case INTERRUPT_CORE_CNTPSIRQ :
    case INTERRUPT_CORE_CNTPNSIRQ:
    case INTERRUPT_CORE_CNTHPIRQ :
    case INTERRUPT_CORE_CNTVIRQ  :
        если (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } иначе {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        вернуть;
    case INTERRUPT_CORE_MAILBOX_0:
    case INTERRUPT_CORE_MAILBOX_1:
    case INTERRUPT_CORE_MAILBOX_2:
    case INTERRUPT_CORE_MAILBOX_3:
        если (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } иначе {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        вернуть;
    case INTERRUPT_CORE_LOCAL_TIMER:
        если (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } иначе {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        вернуть;
    case INTERRUPT_CORE_GPU:
    // Not maskable
    case INTERRUPT_CORE_PMU:
    // Not currently handled
    case INTERRUPT_CORE_AXI:
        // Not currently handled
        вернуть;
    default:
        break;
    }
    если (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        вернуть;
    }

    если (irq < NORMAL_IRQ_OFFSET) {
        если (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } иначе {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } иначе если (irq <= maxIRQ) {
        целое normal_irq = irq - NORMAL_IRQ_OFFSET;
        целое index = normal_irq / 32;
        если (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } иначе {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
