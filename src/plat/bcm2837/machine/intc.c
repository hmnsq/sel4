/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
BOOT_CODE ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
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

BOOT_CODE ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ) {}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ irq_t getActiveIRQ(ಶೂನ್ಯ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    ಯದಿ (pending == 0) {
        ಪ್ರತ್ಯಾಗಮ irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    ಯದಿ (irq != INTERRUPT_CORE_GPU) {
        ಪ್ರತ್ಯಾಗಮ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    ಯದಿ (pending) {
        ಪ್ರತ್ಯಾಗಮ (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ಯದಿ (pending) {
        ಪ್ರತ್ಯಾಗಮ (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ಯದಿ (pending) {
        ಪ್ರತ್ಯಾಗಮ (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ಪ್ರತ್ಯಾಗಮ irqInvalid;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ maskInterrupt(bool_t disable, irq_t irq)
{
    ವಿಕಲ್ಪ (irq) {
    ಪಕ್ಷ INTERRUPT_CORE_CNTPSIRQ :
    ಪಕ್ಷ INTERRUPT_CORE_CNTPNSIRQ:
    ಪಕ್ಷ INTERRUPT_CORE_CNTHPIRQ :
    ಪಕ್ಷ INTERRUPT_CORE_CNTVIRQ  :
        ಯದಿ (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } ಅನ್ಯಥಾ {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ಪ್ರತ್ಯಾಗಮ;
    ಪಕ್ಷ INTERRUPT_CORE_MAILBOX_0:
    ಪಕ್ಷ INTERRUPT_CORE_MAILBOX_1:
    ಪಕ್ಷ INTERRUPT_CORE_MAILBOX_2:
    ಪಕ್ಷ INTERRUPT_CORE_MAILBOX_3:
        ಯದಿ (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } ಅನ್ಯಥಾ {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ಪ್ರತ್ಯಾಗಮ;
    ಪಕ್ಷ INTERRUPT_CORE_LOCAL_TIMER:
        ಯದಿ (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } ಅನ್ಯಥಾ {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ಪ್ರತ್ಯಾಗಮ;
    ಪಕ್ಷ INTERRUPT_CORE_GPU:
    // Not maskable
    ಪಕ್ಷ INTERRUPT_CORE_PMU:
    // Not currently handled
    ಪಕ್ಷ INTERRUPT_CORE_AXI:
        // Not currently handled
        ಪ್ರತ್ಯಾಗಮ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }
    ಯದಿ (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (irq < NORMAL_IRQ_OFFSET) {
        ಯದಿ (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } ಅನ್ಯಥಾ {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } ಅನ್ಯಥಾ ಯದಿ (irq <= maxIRQ) {
        ಪೂರ್ಣಾಙ್ಕ normal_irq = irq - NORMAL_IRQ_OFFSET;
        ಪೂರ್ಣಾಙ್ಕ index = normal_irq / 32;
        ಯದಿ (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } ಅನ್ಯಥಾ {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
