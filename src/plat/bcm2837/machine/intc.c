/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <util.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <plat/machine.h>
BOOT_संहिता शून्य initIRQController(शून्य)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = BIT(व्यवधानम्_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_विस्थापनम्);
    intc_regs->bfEnableBasicIRQs = BIT(व्यवधानम्_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_विस्थापनम्);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_संहिता शून्य cpu_initLocalIRQController(शून्य) {}

स्थिर अन्तःस्थ irq_t get_सक्रिय_IRQ(शून्य)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= आवरणम्(12);
    /* If pending == 0 spurious interrupt */
    यदि (pending == 0) {
        प्रत्यागम irqInvalid;
    }

    /* Get IRQ number */
    irq = (word_द्वयङ्काः - 1 - clzl(pending));
    यदि (irq != व्यवधानम्_CORE_GPU) {
        प्रत्यागम irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(व्यवधानम्_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_विस्थापनम्);
    pending &= ~BIT(व्यवधानम्_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_विस्थापनम्);
    यदि (pending) {
        प्रत्यागम (word_द्वयङ्काः - 1 - clzl(pending)) + BASIC_IRQ_विस्थापनम्;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    यदि (pending) {
        प्रत्यागम (word_द्वयङ्काः - 1 - clzl(pending)) + 32 + प्रसामान्यम्_IRQ_विस्थापनम्;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    यदि (pending) {
        प्रत्यागम (word_द्वयङ्काः - 1 - clzl(pending)) + 0 + प्रसामान्यम्_IRQ_विस्थापनम्;
    }

    प्रत्यागम irqInvalid;
}

स्थिर अन्तःस्थ शून्य आवरणम्_व्यवधानम्(bool_t disable, irq_t irq)
{
    विकल्प (irq) {
    पक्ष व्यवधानम्_CORE_CNTPSIRQ :
    पक्ष व्यवधानम्_CORE_CNTPNSIRQ:
    पक्ष व्यवधानम्_CORE_CNTHPIRQ :
    पक्ष व्यवधानम्_CORE_CNTVIRQ  :
        यदि (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        प्रत्यागम;
    पक्ष व्यवधानम्_CORE_MAILBOX_0:
    पक्ष व्यवधानम्_CORE_MAILBOX_1:
    पक्ष व्यवधानम्_CORE_MAILBOX_2:
    पक्ष व्यवधानम्_CORE_MAILBOX_3:
        यदि (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } अन्यथा {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        प्रत्यागम;
    पक्ष व्यवधानम्_CORE_LOCAL_TIMER:
        यदि (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } अन्यथा {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        प्रत्यागम;
    पक्ष व्यवधानम्_CORE_GPU:
    // Not maskable
    पक्ष व्यवधानम्_CORE_PMU:
    // Not currently handled
    पक्ष व्यवधानम्_CORE_AXI:
        // Not currently handled
        प्रत्यागम;
    स्वभाव:
        भङ्ग;
    }
    यदि (irq < BASIC_IRQ_विस्थापनम्) {
        // Other invalid irq
        प्रत्यागम;
    }

    यदि (irq < प्रसामान्यम्_IRQ_विस्थापनम्) {
        यदि (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_विस्थापनम्);
        } अन्यथा {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_विस्थापनम्);
        }
    } अन्यथा यदि (irq <= maxIRQ) {
        पूर्णाङ्क प्रसामान्यम्_irq = irq - प्रसामान्यम्_IRQ_विस्थापनम्;
        पूर्णाङ्क सूचकांक = प्रसामान्यम्_irq / 32;
        यदि (disable) {
            intc_regs->bfDisableIRQs[सूचकांक] = BIT(प्रसामान्यम्_irq % 32);
        } अन्यथा {
            intc_regs->bfEnableIRQs[सूचकांक] = BIT(प्रसामान्यम्_irq % 32);
        }
    }
}
