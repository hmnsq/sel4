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

स्थिर ओळीत irq_t getActiveIRQ(शून्य)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    जर (pending == 0) {
        परत irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    जर (irq != INTERRUPT_CORE_GPU) {
        परत irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    जर (pending) {
        परत (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    जर (pending) {
        परत (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    जर (pending) {
        परत (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    परत irqInvalid;
}

स्थिर ओळीत शून्य maskInterrupt(bool_t disable, irq_t irq)
{
    निवड (irq) {
    स्थिती INTERRUPT_CORE_CNTPSIRQ :
    स्थिती INTERRUPT_CORE_CNTPNSIRQ:
    स्थिती INTERRUPT_CORE_CNTHPIRQ :
    स्थिती INTERRUPT_CORE_CNTVIRQ  :
        जर (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        परत;
    स्थिती INTERRUPT_CORE_MAILBOX_0:
    स्थिती INTERRUPT_CORE_MAILBOX_1:
    स्थिती INTERRUPT_CORE_MAILBOX_2:
    स्थिती INTERRUPT_CORE_MAILBOX_3:
        जर (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        परत;
    स्थिती INTERRUPT_CORE_LOCAL_TIMER:
        जर (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } अन्यथा {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        परत;
    स्थिती INTERRUPT_CORE_GPU:
    // Not maskable
    स्थिती INTERRUPT_CORE_PMU:
    // Not currently handled
    स्थिती INTERRUPT_CORE_AXI:
        // Not currently handled
        परत;
    पूर्वनिर्धारित:
        थांबा;
    }
    जर (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        परत;
    }

    जर (irq < NORMAL_IRQ_OFFSET) {
        जर (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } अन्यथा {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } अन्यथा जर (irq <= maxIRQ) {
        पूर्णांक normal_irq = irq - NORMAL_IRQ_OFFSET;
        पूर्णांक index = normal_irq / 32;
        जर (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } अन्यथा {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
