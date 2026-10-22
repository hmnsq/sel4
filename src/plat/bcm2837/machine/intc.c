/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <util.h>
#ضمن <stdint.h>
#ضمن <plat/machine.h>
BOOT_کد فراغ initIRQController(فراغ)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = بیت(وقفه_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_جابه_جایی);
    intc_regs->bfEnableBasicIRQs = بیت(وقفه_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_جابه_جایی);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_کد فراغ cpu_initLocalIRQController(فراغ) {}

ساكن مضمن irq_t get_فعال_IRQ(فراغ)
{
    uint32_t pending;
    uint32_t irq;
    /* Read core interrupt register */
    pending = core_regs->coreIRQSource[0];
    /* Mask out invalid bits */
    pending &= ماسک(12);
    /* If pending == 0 spurious interrupt */
    إذا (pending == 0) {
        أرجع irqInvalid;
    }

    /* Get IRQ number */
    irq = (کلمه_بیتها - 1 - clzl(pending));
    إذا (irq != وقفه_CORE_GPU) {
        أرجع irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~بیت(وقفه_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_جابه_جایی);
    pending &= ~بیت(وقفه_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_جابه_جایی);
    إذا (pending) {
        أرجع (کلمه_بیتها - 1 - clzl(pending)) + BASIC_IRQ_جابه_جایی;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    إذا (pending) {
        أرجع (کلمه_بیتها - 1 - clzl(pending)) + 32 + نرمال_IRQ_جابه_جایی;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    إذا (pending) {
        أرجع (کلمه_بیتها - 1 - clzl(pending)) + 0 + نرمال_IRQ_جابه_جایی;
    }

    أرجع irqInvalid;
}

ساكن مضمن فراغ ماسک_وقفه(bool_t disable, irq_t irq)
{
    بدل (irq) {
    حالة وقفه_CORE_CNTPSIRQ :
    حالة وقفه_CORE_CNTPNSIRQ:
    حالة وقفه_CORE_CNTHPIRQ :
    حالة وقفه_CORE_CNTVIRQ  :
        إذا (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~بیت(irq);
        } وإلا {
            core_regs->coreTimersIrqCtrl[0] |= بیت(irq);
        }
        أرجع;
    حالة وقفه_CORE_MAILBOX_0:
    حالة وقفه_CORE_MAILBOX_1:
    حالة وقفه_CORE_MAILBOX_2:
    حالة وقفه_CORE_MAILBOX_3:
        إذا (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~بیت(irq);
        } وإلا {
            core_regs->coreMailboxesIrqCtrl[0] |= بیت(irq);
        }
        أرجع;
    حالة وقفه_CORE_LOCAL_TIMER:
        إذا (disable) {
            core_regs->localTimerCtl &= ~بیت(LOCAL_TIMER_CTRL_IRQ_بیت);
        } وإلا {
            core_regs->localTimerCtl |= بیت(LOCAL_TIMER_CTRL_IRQ_بیت);
        }
        أرجع;
    حالة وقفه_CORE_GPU:
    // Not maskable
    حالة وقفه_CORE_PMU:
    // Not currently handled
    حالة وقفه_CORE_AXI:
        // Not currently handled
        أرجع;
    افتراضي:
        اكسر;
    }
    إذا (irq < BASIC_IRQ_جابه_جایی) {
        // Other invalid irq
        أرجع;
    }

    إذا (irq < نرمال_IRQ_جابه_جایی) {
        إذا (disable) {
            intc_regs->bfDisableBasicIRQs = بیت(irq - BASIC_IRQ_جابه_جایی);
        } وإلا {
            intc_regs->bfEnableBasicIRQs = بیت(irq - BASIC_IRQ_جابه_جایی);
        }
    } وإلا إذا (irq <= maxIRQ) {
        صحيح نرمال_irq = irq - نرمال_IRQ_جابه_جایی;
        صحيح شاخص = نرمال_irq / 32;
        إذا (disable) {
            intc_regs->bfDisableIRQs[شاخص] = بیت(نرمال_irq % 32);
        } وإلا {
            intc_regs->bfEnableIRQs[شاخص] = بیت(نرمال_irq % 32);
        }
    }
}
