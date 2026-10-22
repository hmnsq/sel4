/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine.h>
BOOT_CODE ଶୂନ୍ୟ initIRQController(ଶୂନ୍ୟ)
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

BOOT_CODE ଶୂନ୍ୟ cpu_initLocalIRQController(ଶୂନ୍ୟ) {}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ irq_t getActiveIRQ(ଶୂନ୍ୟ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    ଯଦି (pending == 0) {
        ଫେରନ୍ତୁ irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    ଯଦି (irq != INTERRUPT_CORE_GPU) {
        ଫେରନ୍ତୁ irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    ଯଦି (pending) {
        ଫେରନ୍ତୁ (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    ଯଦି (pending) {
        ଫେରନ୍ତୁ (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    ଯଦି (pending) {
        ଫେରନ୍ତୁ (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    ଫେରନ୍ତୁ irqInvalid;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ maskInterrupt(bool_t disable, irq_t irq)
{
    ବାଛ (irq) {
    ସ୍ଥିତି INTERRUPT_CORE_CNTPSIRQ :
    ସ୍ଥିତି INTERRUPT_CORE_CNTPNSIRQ:
    ସ୍ଥିତି INTERRUPT_CORE_CNTHPIRQ :
    ସ୍ଥିତି INTERRUPT_CORE_CNTVIRQ  :
        ଯଦି (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } ଅନ୍ୟଥା {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        ଫେରନ୍ତୁ;
    ସ୍ଥିତି INTERRUPT_CORE_MAILBOX_0:
    ସ୍ଥିତି INTERRUPT_CORE_MAILBOX_1:
    ସ୍ଥିତି INTERRUPT_CORE_MAILBOX_2:
    ସ୍ଥିତି INTERRUPT_CORE_MAILBOX_3:
        ଯଦି (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } ଅନ୍ୟଥା {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        ଫେରନ୍ତୁ;
    ସ୍ଥିତି INTERRUPT_CORE_LOCAL_TIMER:
        ଯଦି (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } ଅନ୍ୟଥା {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        ଫେରନ୍ତୁ;
    ସ୍ଥିତି INTERRUPT_CORE_GPU:
    // Not maskable
    ସ୍ଥିତି INTERRUPT_CORE_PMU:
    // Not currently handled
    ସ୍ଥିତି INTERRUPT_CORE_AXI:
        // Not currently handled
        ଫେରନ୍ତୁ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }
    ଯଦି (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        ଫେରନ୍ତୁ;
    }

    ଯଦି (irq < NORMAL_IRQ_OFFSET) {
        ଯଦି (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } ଅନ୍ୟଥା {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } ଅନ୍ୟଥା ଯଦି (irq <= maxIRQ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା normal_irq = irq - NORMAL_IRQ_OFFSET;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = normal_irq / 32;
        ଯଦି (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } ଅନ୍ୟଥା {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
