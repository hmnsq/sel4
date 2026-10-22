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
    जइ (pending == 0) {
        प्रत्यागम irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    जइ (irq != INTERRUPT_CORE_GPU) {
        प्रत्यागम irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    जइ (pending) {
        प्रत्यागम (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    जइ (pending) {
        प्रत्यागम (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    जइ (pending) {
        प्रत्यागम (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    प्रत्यागम irqInvalid;
}

स्थिर अन्तःस्थ शून्य maskInterrupt(bool_t disable, irq_t irq)
{
    विकल्प (irq) {
    पक्ष INTERRUPT_CORE_CNTPSIRQ :
    पक्ष INTERRUPT_CORE_CNTPNSIRQ:
    पक्ष INTERRUPT_CORE_CNTHPIRQ :
    पक्ष INTERRUPT_CORE_CNTVIRQ  :
        जइ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } अण्णहा {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        प्रत्यागम;
    पक्ष INTERRUPT_CORE_MAILBOX_0:
    पक्ष INTERRUPT_CORE_MAILBOX_1:
    पक्ष INTERRUPT_CORE_MAILBOX_2:
    पक्ष INTERRUPT_CORE_MAILBOX_3:
        जइ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } अण्णहा {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        प्रत्यागम;
    पक्ष INTERRUPT_CORE_LOCAL_TIMER:
        जइ (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } अण्णहा {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        प्रत्यागम;
    पक्ष INTERRUPT_CORE_GPU:
    // Not maskable
    पक्ष INTERRUPT_CORE_PMU:
    // Not currently handled
    पक्ष INTERRUPT_CORE_AXI:
        // Not currently handled
        प्रत्यागम;
    स्वभाव:
        भङ्ग;
    }
    जइ (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        प्रत्यागम;
    }

    जइ (irq < NORMAL_IRQ_OFFSET) {
        जइ (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } अण्णहा {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } अण्णहा जइ (irq <= maxIRQ) {
        पूर्णाङ्क normal_irq = irq - NORMAL_IRQ_OFFSET;
        पूर्णाङ्क index = normal_irq / 32;
        जइ (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } अण्णहा {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
