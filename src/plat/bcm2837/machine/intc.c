/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <util.h>
#ضمن <stdint.h>
#ضمن <plat/machine.h>
BOOT_CODE فراغ initIRQController(فراغ)
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

BOOT_CODE فراغ cpu_initLocalIRQController(فراغ) {}

ساكن مضمن irq_t getActiveIRQ(فراغ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= MASK(12);
    /* If pending == 0 spurious interrupt */
    إذا (pending == 0) {
        إرجاع irqInvalid;
    }

    /* Get IRQ number */
    irq = (wordBits - 1 - clzl(pending));
    إذا (irq != INTERRUPT_CORE_GPU) {
        إرجاع irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_OFFSET);
    pending &= ~BIT(INTERRUPT_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_OFFSET);
    إذا (pending) {
        إرجاع (wordBits - 1 - clzl(pending)) + BASIC_IRQ_OFFSET;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    إذا (pending) {
        إرجاع (wordBits - 1 - clzl(pending)) + 32 + NORMAL_IRQ_OFFSET;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    إذا (pending) {
        إرجاع (wordBits - 1 - clzl(pending)) + 0 + NORMAL_IRQ_OFFSET;
    }

    إرجاع irqInvalid;
}

ساكن مضمن فراغ maskInterrupt(bool_t disable, irq_t irq)
{
    بدل (irq) {
    حالة INTERRUPT_CORE_CNTPSIRQ :
    حالة INTERRUPT_CORE_CNTPNSIRQ:
    حالة INTERRUPT_CORE_CNTHPIRQ :
    حالة INTERRUPT_CORE_CNTVIRQ  :
        إذا (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~BIT(irq);
        } وإلا {
            core_regs->coreTimersIrqCtrl[0] |= BIT(irq);
        }
        إرجاع;
    حالة INTERRUPT_CORE_MAILBOX_0:
    حالة INTERRUPT_CORE_MAILBOX_1:
    حالة INTERRUPT_CORE_MAILBOX_2:
    حالة INTERRUPT_CORE_MAILBOX_3:
        إذا (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~BIT(irq);
        } وإلا {
            core_regs->coreMailboxesIrqCtrl[0] |= BIT(irq);
        }
        إرجاع;
    حالة INTERRUPT_CORE_LOCAL_TIMER:
        إذا (disable) {
            core_regs->localTimerCtl &= ~BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        } وإلا {
            core_regs->localTimerCtl |= BIT(LOCAL_TIMER_CTRL_IRQ_BIT);
        }
        إرجاع;
    حالة INTERRUPT_CORE_GPU:
    // Not maskable
    حالة INTERRUPT_CORE_PMU:
    // Not currently handled
    حالة INTERRUPT_CORE_AXI:
        // Not currently handled
        إرجاع;
    افتراضي:
        اكسر;
    }
    إذا (irq < BASIC_IRQ_OFFSET) {
        // Other invalid irq
        إرجاع;
    }

    إذا (irq < NORMAL_IRQ_OFFSET) {
        إذا (disable) {
            intc_regs->bfDisableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        } وإلا {
            intc_regs->bfEnableBasicIRQs = BIT(irq - BASIC_IRQ_OFFSET);
        }
    } وإلا إذا (irq <= maxIRQ) {
        صحيح normal_irq = irq - NORMAL_IRQ_OFFSET;
        صحيح index = normal_irq / 32;
        إذا (disable) {
            intc_regs->bfDisableIRQs[index] = BIT(normal_irq % 32);
        } وإلا {
            intc_regs->bfEnableIRQs[index] = BIT(normal_irq % 32);
        }
    }
}
