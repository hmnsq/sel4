/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <util.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <plat/machine.h>
BOOT_CODE শূন্য initIRQController(শূন্য)
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

BOOT_CODE শূন্য cpu_initLocalIRQController(শূন্য) {}

স্থির অন্তঃরেখ irq_t getActiveIRQ(শূন্য)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    যদি (pending == 0) {
        ফেরত irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    যদি (irq != INTERRUPT_CORE_GPU) {
        ফেরত irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    যদি (pending) {
        ফেরত (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    যদি (pending) {
        ফেরত (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    যদি (pending) {
        ফেরত (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ফেরত irqInvalid;
}

স্থির অন্তঃরেখ শূন্য maskInterrupt(bool_t disable, irq_t irq)
{
    নির্বাচন (irq) {
    ক্ষেত্র INTERRUPT_CORE_CNTPSIRQ :
    ক্ষেত্র INTERRUPT_CORE_CNTPNSIRQ:
    ক্ষেত্র INTERRUPT_CORE_CNTHPIRQ :
    ক্ষেত্র INTERRUPT_CORE_CNTVIRQ  :
        যদি (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } অন্যথা {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ফেরত;
    ক্ষেত্র INTERRUPT_CORE_MAILBOX_0:
    ক্ষেত্র INTERRUPT_CORE_MAILBOX_1:
    ক্ষেত্র INTERRUPT_CORE_MAILBOX_2:
    ক্ষেত্র INTERRUPT_CORE_MAILBOX_3:
        যদি (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } অন্যথা {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ফেরত;
    ক্ষেত্র INTERRUPT_CORE_LOCAL_TIMER:
        যদি (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } অন্যথা {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ফেরত;
    ক্ষেত্র INTERRUPT_CORE_GPU:
    // Not maskable
    ক্ষেত্র INTERRUPT_CORE_PMU:
    // Not currently handled
    ক্ষেত্র INTERRUPT_CORE_AXI:
        // Not currently handled
        ফেরত;
    পূর্বনির্ধারিত:
        বিরতি;
    }
    যদি (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ফেরত;
    }

    যদি (irq < NORMAL_IRQ_OFFSET) {
        যদি (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } অন্যথা {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } অন্যথা যদি (irq <= maxIRQ) {
        পূর্ণসংখ্যা normal_irq = irq - NORMAL_IRQ_OFFSET;
        পূর্ণসংখ্যা সূচক = normal_irq / 32;
        যদি (disable) {
            intc_regs->bfDisableIRQs[সূচক] = BIT(normal_irq % 32);
        } অন্যথা {
            intc_regs->bfEnableIRQs[সূচক] = BIT(normal_irq % 32);
        }
    }
}
