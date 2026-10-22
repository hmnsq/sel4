/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <util.h>
#ضمن <stdint.h>
#ضمن <plat/machine.h>
BOOT_کوډ فراغ initIRQController(فراغ)
{
    /* Disable all interrupts */
    intc_regs->bfDisableIRQs[0] = 0xffffffff;
    intc_regs->bfDisableIRQs[1] = 0xffffffff;
    intc_regs->bfDisableBasicIRQs = 0xffffffff;
    /* Disable FIQ */
    intc_regs->FIQ_control &= ~FIQCTRL_FIQ_ENABLE;
    /* Enable IRQ control for GPU */
    intc_regs->bfEnableBasicIRQs = بټ(مداخله_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_دننه_واټن);
    intc_regs->bfEnableBasicIRQs = بټ(مداخله_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_دننه_واټن);

    core_regs->coreTimerPrescaler = 0x80000000;
}

BOOT_کوډ فراغ cpu_initLocalIRQController(فراغ) {}

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
    irq = (کلمه_بټونه - 1 - clzl(pending));
    إذا (irq != مداخله_CORE_GPU) {
        أرجع irq;
    }

    /* GPU interrupt */
    pending = intc_regs->bfIRQBasicPending;
    pending &= intc_regs->bfEnableBasicIRQs;
    /* Mask out pending register 0 and 1 */
    pending &= ~بټ(مداخله_BASIC_IRQ_PENDING_REGISTER1 - BASIC_IRQ_دننه_واټن);
    pending &= ~بټ(مداخله_BASIC_IRQ_PENDING_REGISTER2 - BASIC_IRQ_دننه_واټن);
    إذا (pending) {
        أرجع (کلمه_بټونه - 1 - clzl(pending)) + BASIC_IRQ_دننه_واټن;
    }

    pending = intc_regs->bfGPUIRQPending[1];
    pending &= intc_regs->bfEnableIRQs[1];
    إذا (pending) {
        أرجع (کلمه_بټونه - 1 - clzl(pending)) + 32 + نورمال_IRQ_دننه_واټن;
    }
    pending = intc_regs->bfGPUIRQPending[0];
    pending &= intc_regs->bfEnableIRQs[0];
    إذا (pending) {
        أرجع (کلمه_بټونه - 1 - clzl(pending)) + 0 + نورمال_IRQ_دننه_واټن;
    }

    أرجع irqInvalid;
}

ساكن مضمن فراغ ماسک_مداخله(bool_t disable, irq_t irq)
{
    بدل (irq) {
    حالة مداخله_CORE_CNTPSIRQ :
    حالة مداخله_CORE_CNTPNSIRQ:
    حالة مداخله_CORE_CNTHPIRQ :
    حالة مداخله_CORE_CNTVIRQ  :
        إذا (disable) {
            core_regs->coreTimersIrqCtrl[0] &= ~بټ(irq);
        } وإلا {
            core_regs->coreTimersIrqCtrl[0] |= بټ(irq);
        }
        أرجع;
    حالة مداخله_CORE_MAILBOX_0:
    حالة مداخله_CORE_MAILBOX_1:
    حالة مداخله_CORE_MAILBOX_2:
    حالة مداخله_CORE_MAILBOX_3:
        إذا (disable) {
            core_regs->coreMailboxesIrqCtrl[0] &= ~بټ(irq);
        } وإلا {
            core_regs->coreMailboxesIrqCtrl[0] |= بټ(irq);
        }
        أرجع;
    حالة مداخله_CORE_LOCAL_TIMER:
        إذا (disable) {
            core_regs->localTimerCtl &= ~بټ(LOCAL_TIMER_CTRL_IRQ_بټ);
        } وإلا {
            core_regs->localTimerCtl |= بټ(LOCAL_TIMER_CTRL_IRQ_بټ);
        }
        أرجع;
    حالة مداخله_CORE_GPU:
    // Not maskable
    حالة مداخله_CORE_PMU:
    // Not currently handled
    حالة مداخله_CORE_AXI:
        // Not currently handled
        أرجع;
    افتراضي:
        اكسر;
    }
    إذا (irq < BASIC_IRQ_دننه_واټن) {
        // Other invalid irq
        أرجع;
    }

    إذا (irq < نورمال_IRQ_دننه_واټن) {
        إذا (disable) {
            intc_regs->bfDisableBasicIRQs = بټ(irq - BASIC_IRQ_دننه_واټن);
        } وإلا {
            intc_regs->bfEnableBasicIRQs = بټ(irq - BASIC_IRQ_دننه_واټن);
        }
    } وإلا إذا (irq <= maxIRQ) {
        صحيح نورمال_irq = irq - نورمال_IRQ_دننه_واټن;
        صحيح شاخص = نورمال_irq / 32;
        إذا (disable) {
            intc_regs->bfDisableIRQs[شاخص] = بټ(نورمال_irq % 32);
        } وإلا {
            intc_regs->bfEnableIRQs[شاخص] = بټ(نورمال_irq % 32);
        }
    }
}
