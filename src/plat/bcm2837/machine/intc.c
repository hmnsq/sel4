/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <util.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <plat/machine.h>
BOOT_CODE शून्य initIRQController(शून्य)
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

BOOT_CODE शून्य cpu_initLocalIRQController(शून्य) {}

स्थिर अन्तःस्थ irq_t getActiveIRQ(शून्य)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    सचे (pending == 0) {
        पटिनिवत्त irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    सचे (irq != INTERRUPT_CORE_GPU) {
        पटिनिवत्त irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    सचे (pending) {
        पटिनिवत्त (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    सचे (pending) {
        पटिनिवत्त (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    सचे (pending) {
        पटिनिवत्त (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    पटिनिवत्त irqInvalid;
}

स्थिर अन्तःस्थ शून्य maskInterrupt(bool_t disable, irq_t irq)
{
    विकल्प (irq) {
    पक्ष INTERRUPT_CORE_CNTPSIRQ :
    पक्ष INTERRUPT_CORE_CNTPNSIRQ:
    पक्ष INTERRUPT_CORE_CNTHPIRQ :
    पक्ष INTERRUPT_CORE_CNTVIRQ  :
        सचे (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } अञ्ञथा {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        पटिनिवत्त;
    पक्ष INTERRUPT_CORE_MAILBOX_0:
    पक्ष INTERRUPT_CORE_MAILBOX_1:
    पक्ष INTERRUPT_CORE_MAILBOX_2:
    पक्ष INTERRUPT_CORE_MAILBOX_3:
        सचे (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } अञ्ञथा {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        पटिनिवत्त;
    पक्ष INTERRUPT_CORE_LOCAL_TIMER:
        सचे (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } अञ्ञथा {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        पटिनिवत्त;
    पक्ष INTERRUPT_CORE_GPU:
    // Not maskable
    पक्ष INTERRUPT_CORE_PMU:
    // Not currently handled
    पक्ष INTERRUPT_CORE_AXI:
        // Not currently handled
        पटिनिवत्त;
    स्वभाव:
        भङ्ग;
    }
    सचे (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        पटिनिवत्त;
    }

    सचे (irq < NORMAL_IRQ_OFFSET) {
        सचे (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } अञ्ञथा {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } अञ्ञथा सचे (irq <= maxIRQ) {
        पूर्णाङ्क normal_irq = irq - NORMAL_IRQ_OFFSET;
        पूर्णाङ्क index = normal_irq / 32;
        सचे (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } अञ्ञथा {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
