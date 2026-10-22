/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <plat/machine.h>
BOOT_CODE શૂન્ય initIRQController(શૂન્ય)
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

BOOT_CODE શૂન્ય cpu_initLocalIRQController(શૂન્ય) {}

સ્થિર અંતઃરેખ irq_t getActiveIRQ(શૂન્ય)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    જો (pending == 0) {
        પાછા irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    જો (irq != INTERRUPT_CORE_GPU) {
        પાછા irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    જો (pending) {
        પાછા (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    જો (pending) {
        પાછા (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    જો (pending) {
        પાછા (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    પાછા irqInvalid;
}

સ્થિર અંતઃરેખ શૂન્ય maskInterrupt(bool_t disable, irq_t irq)
{
    પસંદગી (irq) {
    સ્થિતિ INTERRUPT_CORE_CNTPSIRQ :
    સ્થિતિ INTERRUPT_CORE_CNTPNSIRQ:
    સ્થિતિ INTERRUPT_CORE_CNTHPIRQ :
    સ્થિતિ INTERRUPT_CORE_CNTVIRQ  :
        જો (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } નહીંતર {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        પાછા;
    સ્થિતિ INTERRUPT_CORE_MAILBOX_0:
    સ્થિતિ INTERRUPT_CORE_MAILBOX_1:
    સ્થિતિ INTERRUPT_CORE_MAILBOX_2:
    સ્થિતિ INTERRUPT_CORE_MAILBOX_3:
        જો (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } નહીંતર {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        પાછા;
    સ્થિતિ INTERRUPT_CORE_LOCAL_TIMER:
        જો (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } નહીંતર {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        પાછા;
    સ્થિતિ INTERRUPT_CORE_GPU:
    // Not maskable
    સ્થિતિ INTERRUPT_CORE_PMU:
    // Not currently handled
    સ્થિતિ INTERRUPT_CORE_AXI:
        // Not currently handled
        પાછા;
    મૂળભૂત:
        તોડો;
    }
    જો (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        પાછા;
    }

    જો (irq < NORMAL_IRQ_OFFSET) {
        જો (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } નહીંતર {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } નહીંતર જો (irq <= maxIRQ) {
        પૂર્ણાંક normal_irq = irq - NORMAL_IRQ_OFFSET;
        પૂર્ણાંક index = normal_irq / 32;
        જો (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } નહીંતર {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
