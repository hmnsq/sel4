/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <util.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <plat/machine.h>
BOOT_CODE வெறுமை initIRQController(வெறுமை)
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

BOOT_CODE வெறுமை cpu_initLocalIRQController(வெறுமை) {}

நிலையான உள்வரி irq_t getActiveIRQ(வெறுமை)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    எனில் (pending == 0) {
        திரும்பு irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    எனில் (irq != INTERRUPT_CORE_GPU) {
        திரும்பு irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    எனில் (pending) {
        திரும்பு (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    எனில் (pending) {
        திரும்பு (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    எனில் (pending) {
        திரும்பு (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    திரும்பு irqInvalid;
}

நிலையான உள்வரி வெறுமை maskInterrupt(bool_t disable, irq_t irq)
{
    தேர்வு (irq) {
    நிலை INTERRUPT_CORE_CNTPSIRQ :
    நிலை INTERRUPT_CORE_CNTPNSIRQ:
    நிலை INTERRUPT_CORE_CNTHPIRQ :
    நிலை INTERRUPT_CORE_CNTVIRQ  :
        எனில் (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } இல்லையெனில் {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        திரும்பு;
    நிலை INTERRUPT_CORE_MAILBOX_0:
    நிலை INTERRUPT_CORE_MAILBOX_1:
    நிலை INTERRUPT_CORE_MAILBOX_2:
    நிலை INTERRUPT_CORE_MAILBOX_3:
        எனில் (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } இல்லையெனில் {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        திரும்பு;
    நிலை INTERRUPT_CORE_LOCAL_TIMER:
        எனில் (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } இல்லையெனில் {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        திரும்பு;
    நிலை INTERRUPT_CORE_GPU:
    // Not maskable
    நிலை INTERRUPT_CORE_PMU:
    // Not currently handled
    நிலை INTERRUPT_CORE_AXI:
        // Not currently handled
        திரும்பு;
    இயல்பு:
        நிறுத்து;
    }
    எனில் (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        திரும்பு;
    }

    எனில் (irq < NORMAL_IRQ_OFFSET) {
        எனில் (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } இல்லையெனில் {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } இல்லையெனில் எனில் (irq <= maxIRQ) {
        முழு normal_irq = irq - NORMAL_IRQ_OFFSET;
        முழு index = normal_irq / 32;
        எனில் (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } இல்லையெனில் {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
