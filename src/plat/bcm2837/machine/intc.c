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

static anta_hstha irq_t getActiveIRQ(void)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    ᱡᱩᱫᱤ (pending == 0) {
        ᱨᱩᱣᱟᱹᱲ irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    ᱡᱩᱫᱤ (irq != INTERRUPT_CORE_GPU) {
        ᱨᱩᱣᱟᱹᱲ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    ᱡᱩᱫᱤ (pending) {
        ᱨᱩᱣᱟᱹᱲ (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ᱡᱩᱫᱤ (pending) {
        ᱨᱩᱣᱟᱹᱲ (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ᱡᱩᱫᱤ (pending) {
        ᱨᱩᱣᱟᱹᱲ (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ᱨᱩᱣᱟᱹᱲ irqInvalid;
}

static anta_hstha void maskInterrupt(bool_t disable, irq_t irq)
{
    switch (irq) {
    pak_sa INTERRUPT_CORE_CNTPSIRQ :
    pak_sa INTERRUPT_CORE_CNTPNSIRQ:
    pak_sa INTERRUPT_CORE_CNTHPIRQ :
    pak_sa INTERRUPT_CORE_CNTVIRQ  :
        ᱡᱩᱫᱤ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } ᱵᱟᱝ {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ᱨᱩᱣᱟᱹᱲ;
    pak_sa INTERRUPT_CORE_MAILBOX_0:
    pak_sa INTERRUPT_CORE_MAILBOX_1:
    pak_sa INTERRUPT_CORE_MAILBOX_2:
    pak_sa INTERRUPT_CORE_MAILBOX_3:
        ᱡᱩᱫᱤ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } ᱵᱟᱝ {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ᱨᱩᱣᱟᱹᱲ;
    pak_sa INTERRUPT_CORE_LOCAL_TIMER:
        ᱡᱩᱫᱤ (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } ᱵᱟᱝ {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ᱨᱩᱣᱟᱹᱲ;
    pak_sa INTERRUPT_CORE_GPU:
    // Not maskable
    pak_sa INTERRUPT_CORE_PMU:
    // Not currently handled
    pak_sa INTERRUPT_CORE_AXI:
        // Not currently handled
        ᱨᱩᱣᱟᱹᱲ;
    default:
        bha_nga;
    }
    ᱡᱩᱫᱤ (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ᱨᱩᱣᱟᱹᱲ;
    }

    ᱡᱩᱫᱤ (irq < NORMAL_IRQ_OFFSET) {
        ᱡᱩᱫᱤ (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } ᱵᱟᱝ {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } ᱵᱟᱝ ᱡᱩᱫᱤ (irq <= maxIRQ) {
        puur_naa_nka normal_irq = irq - NORMAL_IRQ_OFFSET;
        puur_naa_nka index = normal_irq / 32;
        ᱡᱩᱫᱤ (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } ᱵᱟᱝ {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
