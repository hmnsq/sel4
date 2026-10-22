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
    གལ་སྲིད (pending == 0) {
        ལོག irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    གལ་སྲིད (irq != INTERRUPT_CORE_GPU) {
        ལོག irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    གལ་སྲིད (pending) {
        ལོག (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    གལ་སྲིད (pending) {
        ལོག (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    གལ་སྲིད (pending) {
        ལོག (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ལོག irqInvalid;
}

static anta_hstha void maskInterrupt(bool_t disable, irq_t irq)
{
    switch (irq) {
    pak_sa INTERRUPT_CORE_CNTPSIRQ :
    pak_sa INTERRUPT_CORE_CNTPNSIRQ:
    pak_sa INTERRUPT_CORE_CNTHPIRQ :
    pak_sa INTERRUPT_CORE_CNTVIRQ  :
        གལ་སྲིད (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } དེ་མིན {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ལོག;
    pak_sa INTERRUPT_CORE_MAILBOX_0:
    pak_sa INTERRUPT_CORE_MAILBOX_1:
    pak_sa INTERRUPT_CORE_MAILBOX_2:
    pak_sa INTERRUPT_CORE_MAILBOX_3:
        གལ་སྲིད (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } དེ་མིན {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ལོག;
    pak_sa INTERRUPT_CORE_LOCAL_TIMER:
        གལ་སྲིད (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } དེ་མིན {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ལོག;
    pak_sa INTERRUPT_CORE_GPU:
    // Not maskable
    pak_sa INTERRUPT_CORE_PMU:
    // Not currently handled
    pak_sa INTERRUPT_CORE_AXI:
        // Not currently handled
        ལོག;
    default:
        bha_nga;
    }
    གལ་སྲིད (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ལོག;
    }

    གལ་སྲིད (irq < NORMAL_IRQ_OFFSET) {
        གལ་སྲིད (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } དེ་མིན {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } དེ་མིན གལ་སྲིད (irq <= maxIRQ) {
        ཧྲིལ normal_irq = irq - NORMAL_IRQ_OFFSET;
        ཧྲིལ index = normal_irq / 32;
        གལ་སྲིད (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } དེ་མིན {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
