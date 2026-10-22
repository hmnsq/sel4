/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <util.h>
#समावेश <stdint.h>
#समावेश <plat/machine.h>
BOOT_CODE व्योम initIRQController(व्योम)
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

BOOT_CODE व्योम cpu_initLocalIRQController(व्योम) {}

अटल अंतरभूत irq_t getActiveIRQ(व्योम)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    अगर (pending == 0) {
        वापस irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    अगर (irq != INTERRUPT_CORE_GPU) {
        वापस irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    अगर (pending) {
        वापस (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    अगर (pending) {
        वापस (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    अगर (pending) {
        वापस (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    वापस irqInvalid;
}

अटल अंतरभूत व्योम maskInterrupt(bool_t disable, irq_t irq)
{
    चयन (irq) {
    हाल INTERRUPT_CORE_CNTPSIRQ :
    हाल INTERRUPT_CORE_CNTPNSIRQ:
    हाल INTERRUPT_CORE_CNTHPIRQ :
    हाल INTERRUPT_CORE_CNTVIRQ  :
        अगर (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        वापस;
    हाल INTERRUPT_CORE_MAILBOX_0:
    हाल INTERRUPT_CORE_MAILBOX_1:
    हाल INTERRUPT_CORE_MAILBOX_2:
    हाल INTERRUPT_CORE_MAILBOX_3:
        अगर (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        वापस;
    हाल INTERRUPT_CORE_LOCAL_TIMER:
        अगर (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } अन्यथा {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        वापस;
    हाल INTERRUPT_CORE_GPU:
    // Not maskable
    हाल INTERRUPT_CORE_PMU:
    // Not currently handled
    हाल INTERRUPT_CORE_AXI:
        // Not currently handled
        वापस;
    शेष:
        अवरोध;
    }
    अगर (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        वापस;
    }

    अगर (irq < NORMAL_IRQ_OFFSET) {
        अगर (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } अन्यथा {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } अन्यथा अगर (irq <= maxIRQ) {
        पूर्णांक normal_irq = irq - NORMAL_IRQ_OFFSET;
        पूर्णांक सूचकांक = normal_irq / 32;
        अगर (disable) {
            intc_regs->bfDisableIRQs[सूचकांक] = BIT(normal_irq % 32);
        } अन्यथा {
            intc_regs->bfEnableIRQs[सूचकांक] = BIT(normal_irq % 32);
        }
    }
}
